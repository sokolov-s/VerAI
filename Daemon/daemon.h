#ifndef DAEMON_H
#define DAEMON_H

#include <memory>
#include "torrent.h"
#include "config/config_daemon.h"

namespace daemonspace {

class Daemon
{
public:
    Daemon();

    void Start();
    void Stop();
private:
private:
    std::unique_ptr<torrent::Torrent> torrent;
    const config::ConfigDaemon &daemonCfg;
};

} //namespace daemon
#endif // DAEMON_H
