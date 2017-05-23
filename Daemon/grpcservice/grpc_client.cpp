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
    channel = grpc::CreateChannel(server, grpc::InsecureChannelCredentials());
    stub = DaemonRPC::ServerService::NewStub(channel);

    tasksWorker.Run();
    updateTaskThread = std::thread(&GRPCClient::GetTasks, this);
    isRun = true;
}

void GRPCClient::Stop()
{
    std::lock_guard<std::recursive_mutex> locker(mtxTask);
    if(!IsRun())
        return;
    stub.reset();
    channel.reset();
    tasksWorker.Stop();
    clientInfo.Clear();
    isRun = false;
}

bool GRPCClient::IsRun() const
{
    std::lock_guard<std::recursive_mutex> locker(mtxTask);
    return isRun;
}

void GRPCClient::GetTasks()
{
    while(IsRun()) {
        ClientContext context;
        std::unique_ptr<ClientReader<Task>> reader(
                    stub->GetTaskList(&context, GetClientInfo()));
        auto taskList = ReadStream<Task>(std::move(reader));
        for(const auto &task : taskList) {
            AddTask(task);
        }
        std::this_thread::sleep_for(std::chrono::seconds(updateTimeSec));
    }
}

DaemonInfo GRPCClient::GetClientInfo() const
{
    return clientInfo;
}

void GRPCClient::AddTask(const Task &task)
{
    AddTask(task.task());
}

void GRPCClient::AddTask(const Task::TaskType &taskType)
{
    switch(taskType) {
    case DaemonRPC::Task::TaskType::Task_TaskType_DOWNLOAD_TORRENT:
        tasksWorker.EnqueueTask(common::WrapTask(std::bind(&GRPCClient::DownloadTorrentClb, this)));
        break;

    case DaemonRPC::Task::TaskType::Task_TaskType_GENERATE_MAGENT:
        tasksWorker.EnqueueTask(common::WrapTask(std::bind(&GRPCClient::CreateTorrentClb, this)));
        break;

    case DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS:
        tasksWorker.EnqueueTask(common::WrapTask(std::bind(&GRPCClient::UpdateTorrentClb, this)));
        break;

    default:
        break;
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
