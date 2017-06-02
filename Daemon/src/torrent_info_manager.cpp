#include "torrent_info_manager.h"

using namespace torrent;

std::shared_ptr<TorrentInfo> TorrentInfoManager::UpdateOrAdd(const TorrentInfo &info)
{
    boost::upgrade_lock<boost::shared_mutex> lock(mtx);
    boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);
    auto it = infoList.find(info.GetId());
    if(infoList.end() == it) {
        auto infoPtr = std::make_shared<TorrentInfo>(info);
        it = infoList.emplace(info.GetId(), infoPtr).first;
    } else {
        *it->second = info;
    }
    return it->second;
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
