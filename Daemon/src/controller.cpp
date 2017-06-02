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
    torrent->CreateTorrentAsync(tInfo.id(), tInfo.path());
}

void Controller::DownloadTorrent(const DaemonRPC::TorrentInfo &tInfo)
{
    try {
        torrent->DownloadAsync(tInfo.id(), tInfo.link());
    } catch(const std::runtime_error &) {
        //TODO: send error to the server
        rpcClient->UpdateTorrentStatus(tInfo);
    }
}

DaemonRPC::TorrentInfo Controller::UpdateTorrentInfo(const DaemonRPC::TorrentInfo &tInfo)
{
    auto newInfo = torrent->GetTorrentInfo(tInfo.id());
    return ConvertTorrentInfo(newInfo);
}

DaemonRPC::TorrentInfo Controller::ConvertTorrentInfo(const torrent::TorrentInfo &info) const
{
    DaemonRPC::TorrentInfo resultInfo;
    resultInfo.set_id(info.GetId());
    resultInfo.set_link(info.GetLink());
    resultInfo.set_path(info.GetLink());
    resultInfo.set_status(ConvertTorrentStatus(info.GetStatus()));
    resultInfo.set_workprogress(info.GetProgress());
    return resultInfo;
}

DaemonRPC::TorrentInfo::Status Controller::ConvertTorrentStatus(const torrent::TorrentInfo::Status &status) const
{
    switch (status) {
    case torrent::TorrentInfo::Status::DOWNLOADED:
        return DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_DOWNLOADED;
    case torrent::TorrentInfo::Status::DOWNLOADING:
        return DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_DOWNLOADING;
    case torrent::TorrentInfo::Status::DOWNLOADING_ERROR:
        return DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_ERROR;
    case torrent::TorrentInfo::Status::GENERATED:
        return DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_GENERATED;
    case torrent::TorrentInfo::Status::GENERATING:
        return DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_GENERATING;
    case torrent::TorrentInfo::Status::GENERATION_ERROR:
        return DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_ERROR;
    case torrent::TorrentInfo::Status::PAUSED:
        return DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_PAUSED;
    default:
        return DaemonRPC::TorrentInfo::Status::TorrentInfo_Status_NONE;
    }
}
