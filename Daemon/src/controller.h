#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include "torrent.h"
#include "config/config_daemon.h"
#include "grpcservice/grpc_client.h"

namespace daemonspace {

class Controller : public grpcserver::ClientCallbackInterface
{
public:
    Controller();

    void Start();
    void Stop();

private:
    // ClientCallbackInterface
    virtual std::string GetClientUUID() override;
    virtual void GenerateTorrent(const DaemonRPC::TorrentInfo &tInfo) override;
    virtual void DownloadTorrent(const DaemonRPC::TorrentInfo &tInfo) override;
    virtual DaemonRPC::TorrentInfo UpdateTorrentInfo(const DaemonRPC::TorrentInfo &tInfo) override;

private:
    std::unique_ptr<torrent::Torrent> torrent;
    const config::ConfigDaemon &daemonCfg;
    std::unique_ptr<grpcserver::GRPCClient> rpcClient;
};

} //namespace daemon
#endif // CONTROLLER_H
