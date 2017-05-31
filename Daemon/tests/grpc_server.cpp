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
        info.set_authstatus(DaemonRPC::DaemonInfo::Status::DaemonInfo_Status_OK);
        lock_guard<recursive_mutex> locker(mtx);
        uuids.emplace(daemonUUID);
        static bool isFirst = true;
        if(isFirst) {
            AddTask(daemonUUID, DaemonRPC::Task::TaskType::Task_TaskType_GENERATE_MAGENT);
            AddTask(daemonUUID, DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS);
            isFirst = false;
        } else if (t2Download.status() == DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_GENERATED
                   && !t2Download.link().empty()) {
            AddTask(daemonUUID, DaemonRPC::Task::TaskType::Task_TaskType_DOWNLOAD_TORRENT);
            AddTask(daemonUUID, DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS);
        }
        return info;
    }

    virtual void UpdateTorrentStatus(const DaemonRPC::DaemonInfo &dInfo, const DaemonRPC::TorrentInfo &tInfo) {
        lock_guard<recursive_mutex> locker(mtx);
        std::cout << "Update torrent info for  " << dInfo.uuid() << " : id = " << tInfo.id()
                  << " ; link = " << tInfo.link() << " ; path = " << tInfo.path() << endl;
        if(!tInfo.link().empty() && tInfo.workprogress() == 100 &&
                tInfo.status() == DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_GENERATED) {
            t2Download = tInfo;
            auto it = t2UpdateList.find(dInfo.uuid());
            if (it != t2UpdateList.end()) {
                t2UpdateList.erase(it);
            }
            for(const auto &item : uuids) {
                if(item != dInfo.uuid()) {
                    AddTask(item, DaemonRPC::Task::TaskType::Task_TaskType_DOWNLOAD_TORRENT);
                    AddTask(item, DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS);
                }
            }
        }
    }

    virtual std::vector<DaemonRPC::Task> GetTaskList(const DaemonRPC::DaemonInfo &dInfo) {
        lock_guard<recursive_mutex> locker(mtx);
        auto it = tasks.find(dInfo.uuid());
        std::vector<DaemonRPC::Task> taskList;
        if(it != tasks.end()) {
            taskList.swap(it->second);
            tasks.erase(it);
        }
        return taskList;
    }

    virtual std::vector<DaemonRPC::TorrentInfo> GetInfoForGenerateTorrents(const DaemonRPC::DaemonInfo &dInfo) {
        lock_guard<recursive_mutex> locker(mtx);
        DaemonRPC::TorrentInfo tInfo;
        if(uuids.find(dInfo.uuid()) != uuids.end()) {
            tInfo.set_id(common::GenerateUUID());
            tInfo.set_path("/opt/VerAI/projects/1");
            t2UpdateList[dInfo.uuid()] = tInfo;
        }
        return vector<DaemonRPC::TorrentInfo>({tInfo});
    }

    virtual std::vector<DaemonRPC::TorrentInfo> GetTorrentsForUpdateStatus(const DaemonRPC::DaemonInfo &dInfo) {
        lock_guard<recursive_mutex> locker(mtx);
        DaemonRPC::TorrentInfo tInfo;
        auto item = t2UpdateList.find(dInfo.uuid());
        if(item != t2UpdateList.end()) {
            tInfo = item->second;
        }
        return vector<DaemonRPC::TorrentInfo>({tInfo});
    }

    virtual std::vector<DaemonRPC::TorrentInfo> GetTorrentsForDownload(const DaemonRPC::DaemonInfo &dInfo) {
        lock_guard<recursive_mutex> locker(mtx);
        if(uuids.find(dInfo.uuid()) != uuids.end() && !t2Download.link().empty()) {
            t2UpdateList[dInfo.uuid()] = t2Download;
            return vector<DaemonRPC::TorrentInfo>({t2Download});
        }
        return vector<DaemonRPC::TorrentInfo>();
    }
private:
    std::recursive_mutex mtx;
    set<string> uuids;
    map<string, std::vector<DaemonRPC::Task>> tasks;
    DaemonRPC::TorrentInfo t2Download;
    std::map<string, DaemonRPC::TorrentInfo> t2UpdateList;
};

int main(int /*argc*/, char **/*argv*/)
{
    ServerHandler handler;
    grpcserver::ServerImpl server(&handler);
    server.Start();
    server.Wait();
    return 0;
}
