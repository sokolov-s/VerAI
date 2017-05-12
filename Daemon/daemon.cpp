#include "daemon.h"
#include <plog/Log.h>

using namespace daemonspace;
using namespace std;

Daemon::Daemon()
    : daemonCfg(config::ConfigDaemon::GetInstance())
{
    string logFile = daemonCfg.GetLogFolder() + "/" + "daemon.log";
    plog::init(static_cast<plog::Severity>(LOG_LEVEL), logFile.c_str());
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
