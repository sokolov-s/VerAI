#include "torrent_info_manager.h"
#include <plog/Log.h>

using namespace torrent;

std::shared_ptr<TorrentInfo> TorrentInfoManager::Create(const std::string &id)
{
    return Create(TorrentInfo(id));
}

std::shared_ptr<TorrentInfo> TorrentInfoManager::Create(const TorrentInfo &info)
{
    boost::upgrade_lock<boost::shared_mutex> lock(mtx);
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    auto it = infoList.find(info.GetId());
    if(infoList.end() == it) {
        auto infoPtr = std::make_shared<TorrentInfo>(info);
        it = infoList.emplace(info.GetId(), infoPtr).first;
    }
    return it->second;
}

void TorrentInfoManager::Update(const TorrentInfo &info)
{
    boost::upgrade_lock<boost::shared_mutex> lock(mtx);
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    PLOG_INFO << "Update torrent information : id = " << info.GetId()
               << "; project path = " << info.GetPathToProject()
               << "; torrent file path = " << info.GetPathToTFile()
               << "; torrent link = " << info.GetLink()
               << "; status = " << std::to_string(info.GetStatus())
               << "; work progress = " << std::to_string(info.GetProgress());

    auto it = infoList.find(info.GetId());
    if(infoList.end() != it) {
        *it->second = info;
    }
}

std::shared_ptr<TorrentInfo> TorrentInfoManager::Get(const std::string &id) const
{
    boost::shared_lock<boost::shared_mutex> lock(mtx);
    auto it = infoList.find(id);
    if(infoList.end() != it) {
        return it->second;
    }
    return std::make_shared<TorrentInfo>(id);
}
