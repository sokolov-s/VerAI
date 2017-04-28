#ifndef DAEMON_H
#define DAEMON_H

#include <memory>
#include "torrent.h"
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
};

} //namespace daemon
#endif // DAEMON_H
