#include "grpc_client.h"
#include <memory>

using namespace grpc;
using namespace DaemonRPC;
using namespace grpcserver;

GRPCClient::GRPCClient(ClientCallbackInterface * callbackHandler, const std::string &serverName, const std::string &port)
    : client(callbackHandler)
    , server(serverName + ":" + port)
{
}

void GRPCClient::SetUpdateTaskListTime(std::chrono::seconds sec)
{
    updateTimeSec = sec;
}

void GRPCClient::Start()
{
    std::lock_guard<std::recursive_mutex> locker(mtxTask);
    if(IsRun())
        return;
    //TODO: choose correct safety connection
    channel = grpc::CreateChannel(server, grpc::InsecureChannelCredentials());
    stub = DaemonRPC::ServerService::NewStub(channel);
    tasksWorker.Run();
    isRun = true;
    AddTask(DaemonRPC::Task::TaskType::Task_TaskType_MAKE_HANDSHAKE);
}

void GRPCClient::Stop()
{
    std::lock_guard<std::recursive_mutex> locker(mtxTask);
    if(!IsRun())
        return;
    isRun = false;
    tasksWorker.Stop();
    stub.reset();
    channel.reset();
    clientInfo.Clear();
}

bool GRPCClient::IsRun() const
{
    std::lock_guard<std::recursive_mutex> locker(mtxTask);
    return isRun;
}

void GRPCClient::UpdateTorrentStatus(const TorrentInfo &tInfo)
{
    ClientContext context;
    UpdateTorrentStatusRequest request;
    request.mutable_agentinfo()->CopyFrom(GetClientInfo());
    request.mutable_torrentinfo()->CopyFrom(tInfo);
    BaseResponse response;
    Status status = stub->UpdateTorrentStatus(&context, request, &response);
    if (!status.ok()) {
        PLOG_ERROR << "UpdateTorrentStatus failed : " << status.error_message() << " , error code : " << response.error_code();
    }
}

DaemonInfo GRPCClient::GetClientInfo() const
{
    std::lock_guard<std::recursive_mutex> locker(mtxTask);
    return clientInfo;
}

void GRPCClient::AddTask(const Task &task)
{
    AddTask(task.task());
}

void GRPCClient::AddTask(const Task::TaskType &taskType)
{
    tasksWorker.EnqueueTask(CreateTask(taskType));
}

void GRPCClient::AddTaskAfterTimeOut(const Task::TaskType &taskType, const unsigned int ms)
{
    tasksWorker.EnqueueTaskAfterTimeOut(CreateTask(taskType), ms);
}

std::shared_ptr<common::IRunnable> GRPCClient::CreateTask(const Task::TaskType &taskType)
{
    switch(taskType) {
    case DaemonRPC::Task::TaskType::Task_TaskType_DOWNLOAD_TORRENT:
        return common::WrapTask(std::bind(&GRPCClient::DownloadTorrentClb, this));

    case DaemonRPC::Task::TaskType::Task_TaskType_GENERATE_MAGENT:
        return common::WrapTask(std::bind(&GRPCClient::CreateTorrentClb, this));

    case DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS:
        return common::WrapTask(std::bind(&GRPCClient::UpdateTorrentClb, this));

    case DaemonRPC::Task::TaskType::Task_TaskType_MAKE_HANDSHAKE:
        return common::WrapTask(std::bind(&GRPCClient::HandshakeClb, this));

    case DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TASK_LIST:
        return common::WrapTask(std::bind(&GRPCClient::UpdateTasksClb, this));
        break;

    default:
        return std::shared_ptr<common::IRunnable>();
    }
}

void GRPCClient::UpdateTasksClb()
{
    if(IsRun()) {
        ClientContext context;
        std::unique_ptr<ClientReader<Task>> reader(
                    stub->GetTaskList(&context, GetClientInfo()));
        auto taskList = ReadStream<Task>(std::move(reader));
        for(const auto &task : taskList) {
            AddTask(task);
        }
        AddTaskAfterTimeOut(DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TASK_LIST, updateTimeSec.count() * 1000);
    }
}

void GRPCClient::HandshakeClb()
{
    ClientContext context;
    HandshakeRequest request;
    request.set_uuid(client->GetClientUUID());
    DaemonInfo dInfo;
    Status status = stub->Handshake(&context, request, &dInfo);
    if (!status.ok() || dInfo.authstatus() != DaemonRPC::DaemonInfo::Status::DaemonInfo_Status_OK) {
        PLOG_ERROR << "Handshake failed : " << status.error_message() << " , Authorization status " << dInfo.authstatus();
        AddTaskAfterTimeOut(DaemonRPC::Task::TaskType::Task_TaskType_MAKE_HANDSHAKE, updateTimeSec.count() * 1000);
    } else {
        std::lock_guard<std::recursive_mutex> locker(mtxTask);
        clientInfo = dInfo;
        AddTask(DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TASK_LIST);
    }
}

void GRPCClient::DownloadTorrentClb()
{
    ClientContext context;
    std::unique_ptr<ClientReader<TorrentInfo>> reader(
                stub->GetTorrentsForDownload(&context, GetClientInfo()));
    auto torrentList = ReadStream<TorrentInfo>(std::move(reader));
    client->DownloadTorrent(torrentList);
    AddTask(DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS);
}

void GRPCClient::UpdateTorrentClb()
{
    ClientContext context;
    std::unique_ptr<ClientReader<TorrentInfo>> reader(
                stub->GetTorrentsForUpdateStatus(&context, GetClientInfo()));
    auto torrentList = ReadStream<TorrentInfo>(std::move(reader));
    for(const auto &ti : torrentList) {
        DaemonRPC::TorrentInfo tInfo;
        tInfo = client->UpdateTorrentInfo(ti);
        UpdateTorrentStatus(tInfo);
    }
}

void GRPCClient::CreateTorrentClb()
{
    ClientContext context;
    std::unique_ptr<ClientReader<TorrentInfo>> reader(
                stub->GetInfoForGenerateTorrents(&context, GetClientInfo()));
    auto torrentList = ReadStream<TorrentInfo>(std::move(reader));
    client->GenerateTorrent(torrentList);
    AddTask(DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS);
}
