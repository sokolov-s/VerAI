#include "torrent_info_manager.h"

using namespace torrent;

std::shared_ptr<TorrentInfo> TorrentInfoManager::AddInfo(TorrentInfo &&info)
{
    boost::upgrade_lock<boost::shared_mutex> lock(mtx);
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    auto it = infoList.find(info.GetId());
    if(infoList.end() == it) {
        auto infoPtr = std::make_shared<TorrentInfo>(info);
        auto result = infoList.emplace(info.GetId(), infoPtr);
        it = result.first;
    }
    return it->second;
}

std::shared_ptr<TorrentInfo> TorrentInfoManager::UpdateInfo(const TorrentInfo &info)
{
    boost::shared_lock<boost::shared_mutex> lock(mtx);
    auto it = infoList.find(info.GetId());
    if(infoList.end() != it) {
        *it->second = info;
    }
    return it->second;
}

std::shared_ptr<TorrentInfo> TorrentInfoManager::GetInfo(const std::string &id) const
{
    boost::shared_lock<boost::shared_mutex> lock(mtx);
    auto it = infoList.find(id);
    if(infoList.end() != it) {
        return it->second;
    }
    return std::make_shared<TorrentInfo>(id);
}
