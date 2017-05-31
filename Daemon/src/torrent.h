#ifndef TORRENT_H
#define TORRENT_H

#include "config/config_torrent.h"
#include "torrent_info_manager.h"
#include <memory>
#include <list>
#include <thread>
#include <map>
#include <future>
#include <libtorrent/session.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/alert_types.hpp>

namespace torrent {

class Torrent
{
public:
    Torrent();
    ~Torrent();
    void Start();
    void Stop();
    void CreateTorrentAsync(const std::string &uuid, const std::string &path);

    TorrentInfo GetTorrentInfo(const std::string &uuid) const;
    std::string GetMagnet(const std::string &uuid) const;
    void DownloadAsync(const std::string &uuid, const std::string &link) noexcept(false);

private:
    void Handler();
    std::string State(libtorrent::torrent_status::state_t s);
    std::string GetResumeFilePath(const libtorrent::add_torrent_params &param) const;
    std::string GetResumeFilePath(const std::string &name) const;
    void CreateTorrent(const std::string &uuid, const std::string &path);
    void UpdateCreationProgress(const std::string &uuid, int curPiece, int totalPieces);
    void FindTFilesAndAdd();
    void AddTorrent(const std::string &uuid, const std::string &fullPath) noexcept(false);
    void AddTorrent(const std::string &uuid, libtorrent::add_torrent_params && param);
    bool IsWork() const;
    void UpdateStatus(const std::string &uuid, const TorrentInfo::Status &status, const uint progress);
    std::string GetIdByName(const std::string &name) const;
private:
    const config::ConfigTorrent &cfg;
    std::unique_ptr<libtorrent::session> session;
    struct TorrentsInSystem {
        libtorrent::add_torrent_params param;
        libtorrent::torrent_alert const * handler = nullptr;
    };
    std::thread handlersThread;
    std::thread addThread;
    mutable std::recursive_mutex workMtx;
    mutable std::recursive_mutex torrentIdListMtx;
    bool isWork = false;
    std::map<std::string, TorrentsInSystem> torrentsIdList;
    TorrentInfoManager infoManager;
};

} //namespace torrent
#endif // TORRENT_H
