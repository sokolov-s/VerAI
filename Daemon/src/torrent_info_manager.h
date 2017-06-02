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
    std::shared_ptr<TorrentInfo> UpdateOrAdd(const TorrentInfo &info);
    std::shared_ptr<TorrentInfo> Get(const std::string &id) const;
private:
    mutable boost::shared_mutex mtx;
    typedef std::map<std::string, std::shared_ptr<TorrentInfo>> InfoList;
    InfoList infoList;
};

} //namespace torrent
#endif // TORRENT_INFO_MANAGER_H
