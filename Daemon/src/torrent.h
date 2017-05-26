#ifndef TORRENT_H
#define TORRENT_H

#include "config/config_torrent.h"
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
    void CreateTorrentAsync(const std::string &path, const std::string &uuid);
    struct CreatingTorrentStatus {
        enum Status {
            UNKNOWN,
            IN_PROGRESS,
            READY,
            FAILED,
        };
        CreatingTorrentStatus(Status st, int pr = 0) : status(st), progress(pr){}
        Status status;
        short int progress = 0;
    };
    CreatingTorrentStatus GetCreationTorrentStatus(const std::string &uuid) const;
    std::string GetMagnet(const std::string &uuid) const;
    void DownloadAsync(const std::string &link) throw();

private:
    void Handler();
    std::string State(libtorrent::torrent_status::state_t s);
    std::string GetResumeFilePath(const libtorrent::add_torrent_params &param) const;
    std::string GetResumeFilePath(const std::string &name) const;
    void CreateTorrent(const std::string &path, const std::string &uuid = "");
    void FindTFilesAndAdd();
    void AddTorrent(const std::string &fullPath, const std::string &uuid = "") throw();
    void AddTorrent(libtorrent::add_torrent_params && param, const std::string &uuid = "");
    bool IsWork() const;
    void SetCreationTorrentStatus(const std::string &uuid, const CreatingTorrentStatus &status);
    void UpdateCreationTorrentStatus(const std::string &uuid, CreatingTorrentStatus::Status status);
    void UpdateCreationTorrentProgress(const std::string &uuid, int curPiece, int totalPieces);
private:
    const config::ConfigTorrent &cfg;
    std::unique_ptr<libtorrent::session> session;
    typedef std::list<libtorrent::add_torrent_params> params_type;
    params_type params;
    std::thread handlersThread;
    std::thread addThread;
    mutable std::recursive_mutex workMtx;
    mutable std::mutex paramsMtx;
    bool isWork = false;
    std::map<std::string, libtorrent::torrent_alert const *> tHandlers;
    std::map<std::string, params_type::iterator> torrents;
    mutable std::mutex prepareMagnetStatusMtx;
    std::map<std::string, CreatingTorrentStatus> prepareMagnetStatusList;
};

} //namespace torrent
#endif // TORRENT_H
