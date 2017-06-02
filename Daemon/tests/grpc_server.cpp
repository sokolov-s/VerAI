#include "grpcservice/grpc_server.h"
#include "common/uuid.h"
#include <string>
#include <iostream>
#include <map>
#include <deque>
#include <set>
#include <vector>
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
        } else if(!t2Download.empty()) {
            Add2DownloadList(daemonUUID, t2Download.at(0));
            t2Download.pop_front();
            AddTask(daemonUUID, DaemonRPC::Task::TaskType::Task_TaskType_DOWNLOAD_TORRENT);
        }
        return info;
    }

    virtual void UpdateTorrentStatus(const DaemonRPC::DaemonInfo &dInfo, const DaemonRPC::TorrentInfo &tInfo) {
        lock_guard<recursive_mutex> locker(mtx);
        std::cout << "Update torrent info for  " << dInfo.uuid()
                  << " : id = " << tInfo.id()
                  << " ; link = " << tInfo.link() << " ; path = " << tInfo.path()
                  << " ; status = " << to_string(tInfo.status())
                  << " progress = " << to_string(tInfo.workprogress()) << endl;
        switch(tInfo.status()) {
        case DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_GENERATED:
            if(!tInfo.link().empty()) {
                t2Download.push_back(tInfo);
                RemoveFromUpdateList(dInfo.uuid(), tInfo);
                for(const auto &item : uuids) {
                    if(item != dInfo.uuid()) {
                        Add2DownloadList(item, t2Download.at(0));
                        t2Download.pop_front();
                        AddTask(item, DaemonRPC::Task::TaskType::Task_TaskType_DOWNLOAD_TORRENT);
                    }
                }
            }
            break;

        case DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_DOWNLOADED:
            RemoveFromUpdateList(dInfo.uuid(), tInfo);
            break;

        case DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_DOWNLOADING:
        case DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_GENERATING:
            AddTask(dInfo.uuid(), DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS);
            break;

        default:
            break;
        }
    }

    virtual std::vector<DaemonRPC::Task> GetTaskList(const DaemonRPC::DaemonInfo &dInfo) {
        lock_guard<recursive_mutex> locker(mtx);
        auto it = tasks.find(dInfo.uuid());
        std::vector<DaemonRPC::Task> taskList;
        if(it != tasks.end()) {
            taskList.swap(it->second);
            tasks.erase(it);
            std::cout << "GetTaskList for  " << dInfo.uuid() << " daemon, tasks count : "
                      << to_string(taskList.size()) << endl;
        }
        return taskList;
    }

    virtual std::vector<DaemonRPC::TorrentInfo> GetInfoForGenerateTorrents(const DaemonRPC::DaemonInfo &dInfo) {
        lock_guard<recursive_mutex> locker(mtx);
        DaemonRPC::TorrentInfo tInfo;
        if(uuids.find(dInfo.uuid()) != uuids.end()) {
            tInfo.set_id(common::GenerateUUID());
            tInfo.set_path("/opt/VerAI/projects/1");
            Add2UpdateList(dInfo.uuid(), tInfo);
            std::cout << "GetInfoForGenerateTorrents for  " << dInfo.uuid() << " daemon, torrent info : "
                      << tInfo.id() <<  " , path = " << tInfo.path() << endl;
        }
        return vector<DaemonRPC::TorrentInfo>({tInfo});
    }

    virtual std::vector<DaemonRPC::TorrentInfo> GetTorrentsForUpdateStatus(const DaemonRPC::DaemonInfo &dInfo) {
        lock_guard<recursive_mutex> locker(mtx);
        auto it = t2UpdateList.find(dInfo.uuid());
        if(t2UpdateList.end() != it) {
            std::cout << "GetTorrentsForUpdateStatus for  " << dInfo.uuid() << " daemon, torrents size : "
                      << to_string(it->second.size()) << endl;
            return it->second;
        }
        return vector<DaemonRPC::TorrentInfo>();
    }

    virtual std::vector<DaemonRPC::TorrentInfo> GetTorrentsForDownload(const DaemonRPC::DaemonInfo &dInfo) {
        lock_guard<recursive_mutex> locker(mtx);
        auto it = t2DownloadList.find(dInfo.uuid());
        if(t2DownloadList.end() != it) {
            std::vector<DaemonRPC::TorrentInfo> res;
            for(const auto &item : it->second) {
                Add2UpdateList(dInfo.uuid(), item);
                res.push_back(item);
            }
            t2DownloadList.erase(it);
            AddTask(dInfo.uuid(), DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS);
            std::cout << "GetTorrentsForDownload for  " << dInfo.uuid() << " daemon, torrents size : "
                      << to_string(res.size()) << endl;
            return res;
        }
        return vector<DaemonRPC::TorrentInfo>();
    }
private:
    void Add2UpdateList(const std::string &id, const DaemonRPC::TorrentInfo &info) {
        lock_guard<recursive_mutex> locker(mtx);
        map<string, vector<DaemonRPC::TorrentInfo>>::iterator it = t2UpdateList.find(id);
        if(t2UpdateList.end() == it) {
            it = (t2UpdateList.emplace(id, vector<DaemonRPC::TorrentInfo>({}))).first;
        }
        it->second.push_back(info);
    }

    void RemoveFromUpdateList(const std::string &id, const DaemonRPC::TorrentInfo &info) {
        lock_guard<recursive_mutex> locker(mtx);
        auto it = t2UpdateList.find(id);
        if(it != t2UpdateList.end()) {
            for(auto infoIt = it->second.begin(); infoIt != it->second.end(); ++infoIt) {
                if(infoIt->id() == info.id()) {
                    it->second.erase(infoIt);
                    break;
                }
            }
        }
    }

    void Add2DownloadList(const std::string &id, const DaemonRPC::TorrentInfo &info) {
        lock_guard<recursive_mutex> locker(mtx);
        map<string, vector<DaemonRPC::TorrentInfo>>::iterator it = t2DownloadList.find(id);
        if(t2DownloadList.end() == it) {
            it = (t2DownloadList.emplace(id, vector<DaemonRPC::TorrentInfo>())).first;
        }
        it->second.push_back(info);
    }
private:
    recursive_mutex mtx;
    set<string> uuids;
    deque<DaemonRPC::TorrentInfo> t2Download;
    map<string, std::vector<DaemonRPC::Task>> tasks;
    map<string, vector<DaemonRPC::TorrentInfo>> t2DownloadList;
    map<string, vector<DaemonRPC::TorrentInfo>> t2UpdateList;
};

int main(int /*argc*/, char **/*argv*/)
{
    ServerHandler handler;
    grpcserver::ServerImpl server(&handler);
    server.Start();
    server.Wait();
    return 0;
}
