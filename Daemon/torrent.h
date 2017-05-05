#ifndef TORRENT_H
#define TORRENT_H

#include "config/config_torrent.h"

namespace torrent {

class Torrent
{
public:
    Torrent();
    void Start();
private:
    const config::ConfigTorrent &cfg;
};

} //namespace torrent
#endif // TORRENT_H
