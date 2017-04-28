#ifndef TORRENT_H
#define TORRENT_H

#include "config.h"

namespace torrent {

class Torrent
{
public:
    Torrent();
    void Start();
private:
    const config::Torrent &cfg;
};

} //namespace torrent
#endif // TORRENT_H
