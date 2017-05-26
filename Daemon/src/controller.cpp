#include "controller.h"
#include <plog/Log.h>

using namespace daemonspace;
using namespace std;

Controller::Controller()
    : daemonCfg(config::ConfigDaemon::GetInstance())
{
    string logFile = daemonCfg.GetLogFolder() + "/" + "daemon.log";
    plog::init(static_cast<plog::Severity>(LOG_LEVEL), logFile.c_str(), 10 * 1024 * 1024, INT_MAX);
}

void Controller::Start()
{
    torrent.reset(new torrent::Torrent());
    torrent->Start();
    rpcClient.reset(new grpcserver::GRPCClient(this, daemonCfg.GetServerDomainName(), daemonCfg.GetServerPort()));
    rpcClient->Start();
}

void Controller::Stop()
{
    torrent.release();
    rpcClient.reset();
}

string Controller::GetClientUUID()
{
    return daemonCfg.GetUUID();
}

void Controller::GenerateTorrent(const DaemonRPC::TorrentInfo &tInfo)
{
    torrent->CreateTorrentAsync(tInfo.path(), tInfo.id());
}

void Controller::DownloadTorrent(const DaemonRPC::TorrentInfo &tInfo)
{
    torrent->DownloadAsync(tInfo.link());
}

DaemonRPC::TorrentInfo Controller::UpdateTorrentInfo(const DaemonRPC::TorrentInfo &tInfo)
{
    //TODO: write correct code to update torrent status
    return tInfo;
}
