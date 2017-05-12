#ifndef TORRENT_H
#define TORRENT_H

#include "config/config_torrent.h"
#include <memory>
#include <vector>
#include <thread>
#include <map>
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
private:
    void Handler();
    std::string State(libtorrent::torrent_status::state_t s);
private:
    const config::ConfigTorrent &cfg;
    std::unique_ptr<libtorrent::session> session;
    std::vector<libtorrent::add_torrent_params> params;
    std::thread handlersThread;
    std::mutex handlersMtx;
    bool isWork = false;
    std::map<std::string, libtorrent::torrent_alert const *> tHandlers;
};

} //namespace torrent
#endif // TORRENT_H
