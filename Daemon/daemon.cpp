#include "daemon.h"

using namespace daemonspace;
using namespace std;

Daemon::Daemon()
{
}

void Daemon::Start()
{
    torrent.reset(new torrent::Torrent());
    torrent->Start();
}

void Daemon::Stop()
{
    torrent.release();
}
