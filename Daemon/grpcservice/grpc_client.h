#ifndef GRPC_CLIENT_H
#define GRPC_CLIENT_H

#include <string>
#include <memory>
#include <functional>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include "common/working_thread.h"
#include <grpc++/grpc++.h>
#include <daemon.grpc.pb.h>
#include <plog/Log.h>

namespace grpcserver {

class ClientCallbackInterface {
public:
    virtual ~ClientCallbackInterface() {}
    virtual std::string GetClientUUID() = 0;
    virtual void GenerateTorrent(const DaemonRPC::TorrentInfo &tInfo) = 0;
    virtual void GenerateTorrent(const std::vector<DaemonRPC::TorrentInfo> &tInfoList) {
        for(const auto t : tInfoList) {
            GenerateTorrent(t);
        }
    }

    virtual void DownloadTorrent(const DaemonRPC::TorrentInfo &torrent) = 0;
    virtual void DownloadTorrent(const std::vector<DaemonRPC::TorrentInfo> &torrents) {
        for(const auto t : torrents) {
            DownloadTorrent(t);
        }
    }

    virtual DaemonRPC::TorrentInfo UpdateTorrentInfo(const DaemonRPC::TorrentInfo &tInfo) = 0;
};

class GRPCClient
{
public:
    GRPCClient(ClientCallbackInterface * callbackHandler, const std::string &serverName, const std::string &port);
    void SetUpdateTaskListTime(std::chrono::seconds sec);
    void Start();
    void Stop();
    bool IsRun() const;
    void UpdateTorrentStatus(const DaemonRPC::TorrentInfo &tInfo);

private:
    void GetTasks();
    DaemonRPC::DaemonInfo GetClientInfo() const;
    void AddTask(const DaemonRPC::Task &task);
    void AddTask(const DaemonRPC::Task::TaskType &taskType);
    void HandShake();
    void DownloadTorrentClb();
    void UpdateTorrentClb();
    void CreateTorrentClb();

    template<typename T>
    std::vector<T> ReadStream(std::unique_ptr<grpc::ClientReader<T>> &&reader) {
        T data;
        std::vector<T> dataList;
        while(reader->Read(&data)) {
            dataList.push_back(data);
        }
        grpc::Status status = reader->Finish();
        if (!status.ok()) {
            PLOG_ERROR << typeid(T).name() <<" rpc failed";
        }
        return dataList;
    }

private:
    ClientCallbackInterface *client = nullptr;
    std::string server;
    std::shared_ptr<grpc::Channel> channel;
    std::unique_ptr<DaemonRPC::ServerService::Stub> stub;
    mutable std::recursive_mutex mtxTask;
    std::chrono::seconds updateTimeSec = std::chrono::seconds(30);
    bool isRun = false;
    DaemonRPC::DaemonInfo clientInfo;
    common::WorkingThread tasksWorker;
    std::thread updateTaskThread;
};

} //namespace grpcservice

#endif // GRPC_CLIENT_H
