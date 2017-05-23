#include "grpcservice/grpc_server.h"
#include "common/uuid.h"
#include <string>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <memory>

using namespace std;

class ServerHandler : public grpcserver::ServerHandlerInterface
{
public:
    void AddTask(const std::string &daemonUUID, const DaemonRPC::Task::TaskType &taskType) {
        lock_guard<recursive_mutex> locker(mtx);
        auto it = tasks.find(daemonUUID);
        DaemonRPC::Task task;
        task.set_task(taskType);
        if(it != tasks.end()) {
            it->second.push_back(task);
        } else {
            std::vector<DaemonRPC::Task> taskList = {task};
            tasks[daemonUUID] = taskList;
        }
    }

    virtual DaemonRPC::DaemonInfo Handshake(const std::string &daemonUUID) {
        DaemonRPC::DaemonInfo info;
        info.set_uuid(daemonUUID);
        info.set_authtocken("Some tocken");
        lock_guard<recursive_mutex> locker(mtx);
        uuids.emplace(daemonUUID);
        static bool isFirst = true;
        if(isFirst) {
            AddTask(daemonUUID, DaemonRPC::Task::TaskType::Task_TaskType_GENERATE_MAGENT);
            isFirst = false;
        }
        return info;
    }

    virtual void UpdateTorrentStatus(const DaemonRPC::DaemonInfo &dInfo, const DaemonRPC::TorrentInfo &tInfo) {
        std::cout << "Update torrent info for  " << dInfo.uuid() << " : id = " << tInfo.id()
                  << " ; link = " << tInfo.link() << " ; path = " << tInfo.path() << endl;
        if(!tInfo.link().empty()) {
            t2Download = tInfo;
            for(const auto &item : uuids) {
                if(item != dInfo.uuid()) {
                    AddTask(item, DaemonRPC::Task::TaskType::Task_TaskType_DOWNLOAD_TORRENT);
                }
            }
        }
    }

    virtual std::vector<DaemonRPC::Task> GetTaskList(const DaemonRPC::DaemonInfo &dInfo) {
        auto it = tasks.find(dInfo.uuid());
        std::vector<DaemonRPC::Task> taskList;
        if(it != tasks.end()) {
            taskList.swap(it->second);
            tasks.erase(it);
        }
        return taskList;
    }

    virtual std::vector<DaemonRPC::TorrentInfo> GetInfoForGenerateTorrents(const DaemonRPC::DaemonInfo &dInfo) {
        DaemonRPC::TorrentInfo tInfo;
        if(uuids.find(dInfo.uuid()) != uuids.end()) {
            tInfo.set_id(common::GenerateUUID());
            tInfo.set_path("/opt/VeraAI/projects/1");
        }
        return vector<DaemonRPC::TorrentInfo>({tInfo});
    }

    virtual std::vector<DaemonRPC::TorrentInfo> GetTorrentsForDownload(const DaemonRPC::DaemonInfo &dInfo) {
        if(uuids.find(dInfo.uuid()) != uuids.end() && !t2Download.link().empty()) {
            return vector<DaemonRPC::TorrentInfo>({t2Download});
        }
        return vector<DaemonRPC::TorrentInfo>();
    }
private:
    std::recursive_mutex mtx;
    set<string> uuids;
    map<string, std::vector<DaemonRPC::Task>> tasks;
    DaemonRPC::TorrentInfo t2Download;
};

int main(int /*argc*/, char **/*argv*/)
{
    ServerHandler handler;
    grpcserver::ServerImpl server(&handler);
    server.Start();
    server.Wait();
    return 0;
}
