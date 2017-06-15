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
    typedef deque<DaemonRPC::TorrentInfo> TInfoDeque;
    typedef vector<DaemonRPC::TorrentInfo> TInfoVector;
    typedef map<string, TInfoVector> DaemonTInfoMap;
    typedef map<string, std::vector<DaemonRPC::Task>> DaemonTasks;

public:
    void AddTask(const std::string &daemonUUID, const DaemonRPC::Task::TaskType &taskType) {
        lock_guard<recursive_mutex> locker(mtxTDeque);
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
        lock_guard<recursive_mutex> locker(mtxTDeque);
        uuids.emplace(daemonUUID);
        static bool isFirst = true;
        if(isFirst) {
            AddTask(daemonUUID, DaemonRPC::Task::TaskType::Task_TaskType_GENERATE_MAGENT);
            AddTask(daemonUUID, DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS);
            isFirst = false;
        } else {
            DownloadTorrent(daemonUUID);
        }
        return info;
    }

    virtual void UpdateTorrentStatus(const DaemonRPC::DaemonInfo &dInfo, const DaemonRPC::TorrentInfo &tInfo) {
        lock_guard<recursive_mutex> locker(mtxTDeque);
        std::cout << "Update torrent info for  " << dInfo.uuid()
                  << " : id = " << tInfo.id()
                  << " ; link = " << tInfo.link()
                  << " ; path to torrent = " << tInfo.pathtotfile()
                  << " ; path to project = " << tInfo.pathtoproject()
                  << " ; status = " << to_string(tInfo.status())
                  << " ; progress = " << to_string(tInfo.workprogress()) << endl;

        switch(tInfo.status()) {
        case DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_GENERATED:
            if(!tInfo.link().empty()) {
                torrentDeque.push_back(tInfo);
                RemoveFromUpdateList(dInfo.uuid(), tInfo);
                for(const auto &daemonId : uuids) {
                    if(daemonId != dInfo.uuid()) {
                        DownloadTorrent(daemonId);
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
            AddTask(dInfo.uuid(), DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS);
            break;
        }
    }

    virtual std::vector<DaemonRPC::Task> GetTaskList(const DaemonRPC::DaemonInfo &dInfo) {
        lock_guard<recursive_mutex> locker(mtxTDeque);
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
        lock_guard<recursive_mutex> locker(mtxTDeque);
        DaemonRPC::TorrentInfo tInfo;
        if(uuids.find(dInfo.uuid()) != uuids.end()) {
            tInfo.set_id(common::GenerateUUID());
            tInfo.set_pathtoproject("/opt/VerAI/projects/1");
            Add2UpdateList(dInfo.uuid(), tInfo);
            std::cout << "GetInfoForGenerateTorrents for  " << dInfo.uuid() << " daemon, torrent info : "
                      << tInfo.id() <<  " , path to project = " << tInfo.pathtoproject() << endl;
        }
        return vector<DaemonRPC::TorrentInfo>({tInfo});
    }

    virtual std::vector<DaemonRPC::TorrentInfo> GetTorrentsForUpdateStatus(const DaemonRPC::DaemonInfo &dInfo) {
        lock_guard<recursive_mutex> locker(mtxTUpdate);
        auto it = t2UpdateMap.find(dInfo.uuid());
        if(t2UpdateMap.end() != it) {
            std::cout << "GetTorrentsForUpdateStatus for  " << dInfo.uuid() << " daemon, torrents size : "
                      << to_string(it->second.size()) << endl;
            return it->second;
        }
        return vector<DaemonRPC::TorrentInfo>();
    }

    virtual std::vector<DaemonRPC::TorrentInfo> GetTorrentsForDownload(const DaemonRPC::DaemonInfo &dInfo) {
        lock_guard<recursive_mutex> locker(mtxTDownload);
        auto it = t2DownloadMap.find(dInfo.uuid());
        if(t2DownloadMap.end() != it) {
            std::vector<DaemonRPC::TorrentInfo> res;
            for(const auto &item : it->second) {
                Add2UpdateList(dInfo.uuid(), item);
                res.push_back(item);
            }
            t2DownloadMap.erase(it);
            AddTask(dInfo.uuid(), DaemonRPC::Task::TaskType::Task_TaskType_UPDATE_TORRENT_STATUS);
            std::cout << "GetTorrentsForDownload for  " << dInfo.uuid() << " daemon, torrents size : "
                      << to_string(res.size()) << endl;
            return res;
        }
        return vector<DaemonRPC::TorrentInfo>();
    }
private:
    void Add2UpdateList(const std::string &id, const DaemonRPC::TorrentInfo &info) {
        lock_guard<recursive_mutex> locker(mtxTUpdate);
        UpdateOrAddTorrent(id, t2UpdateMap, info);
    }

    void RemoveFromUpdateList(const std::string &id, const DaemonRPC::TorrentInfo &info) {
        lock_guard<recursive_mutex> locker(mtxTUpdate);
        RemoveTorrent(id, t2UpdateMap, info);
    }

    void Add2DownloadList(const std::string &id, const DaemonRPC::TorrentInfo &info) {
        lock_guard<recursive_mutex> locker(mtxTDownload);
        UpdateOrAddTorrent(id, t2DownloadMap, info);
    }

    DaemonTInfoMap::iterator UpdateOrAddTorrent(const std::string &id, DaemonTInfoMap &infoMap, const DaemonRPC::TorrentInfo &info) {
        auto it = infoMap.find(id);
        if(infoMap.end() == it) {
            return infoMap.emplace(id, TInfoVector({info})).first;
        } else {
            auto infoVector = FindTorrent(it->second, info);
            if(it->second.end() != infoVector) {
                *infoVector = info;
            } else {
                it->second.push_back(info);
            }
            return it;
        }
    }

    void RemoveTorrent(const std::string &id, DaemonTInfoMap &infoMap, const DaemonRPC::TorrentInfo &info) {
        auto it = infoMap.find(id);
        if(it != infoMap.end()) {
            RemoveTorrent(it->second, info);
        }
    }

    void RemoveTorrent(TInfoVector &list, const DaemonRPC::TorrentInfo &info) {
        auto it = FindTorrent(list, info);
        if(list.end() != it) {
            list.erase(it);
        }
    }

    TInfoVector::iterator FindTorrent(TInfoVector &list, const DaemonRPC::TorrentInfo &info) {
        for(TInfoVector::iterator it = list.begin();
            it != list.end(); ++it) {
            if(info.id() == it->id()) {
                return it;
            }
        }
        return list.end();
    }

    void DownloadTorrent(const std::string daemonId) {
        lock_guard<recursive_mutex> locker(mtxTDeque);
        if(!torrentDeque.empty()) {
            Add2DownloadList(daemonId, torrentDeque.at(0));
            torrentDeque.pop_front();
            AddTask(daemonId, DaemonRPC::Task::TaskType::Task_TaskType_DOWNLOAD_TORRENT);
        }
    }

private:
    recursive_mutex mtxTDeque;
    recursive_mutex mtxTDownload;
    recursive_mutex mtxTUpdate;
    set<string> uuids;
    TInfoDeque torrentDeque;
    DaemonTasks tasks;
    DaemonTInfoMap t2DownloadMap;
    DaemonTInfoMap t2UpdateMap;
};

int main(int /*argc*/, char **/*argv*/)
{
    ServerHandler handler;
    grpcserver::ServerImpl server(&handler);
    server.Start();
    server.Wait();
    return 0;
}
