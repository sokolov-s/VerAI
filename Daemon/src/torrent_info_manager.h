#ifndef TORRENT_INFO_MANAGER_H
#define TORRENT_INFO_MANAGER_H

#include "torrent_info.h"
#include <boost/thread.hpp>
#include <map>
#include <memory>

namespace torrent {

class TorrentInfoManager
{
public:
    std::shared_ptr<TorrentInfo> AddInfo(TorrentInfo &&info);
    std::shared_ptr<TorrentInfo> UpdateInfo(const TorrentInfo &info);
    std::shared_ptr<TorrentInfo> GetInfo(const std::string &id) const;
private:
    mutable boost::shared_mutex mtx;
    std::map<std::string, std::shared_ptr<TorrentInfo>> infoList;
};

} //namespace torrent
#endif // TORRENT_INFO_MANAGER_H
