#ifndef GRPC_SERVER_H
#define GRPC_SERVER_H

#include <string>
#include <memory>
#include <map>

#include <grpc++/grpc++.h>
#include "daemon.grpc.pb.h"

namespace grpcserver {

class ClientHandlerInterface;
class ServerHandlerInterface;

template<typename T>
class ServerHelper
{
public:
    ServerHelper(T *handlerObject) : handler(handlerObject) {}
    ~ServerHelper() {
        Stop();
    }

    void Start() {
        std::string server_address("0.0.0.0:50051");
        grpc::ServerBuilder builder;
        builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
        builder.RegisterService(this);
        server = builder.BuildAndStart();
    }

    void Stop() {
        if(server) {
            server->Shutdown();
            server->Wait();
            server.release();
        }
    }

    T *GetHandler() {
        return handler;
    }

private:
    std::unique_ptr<grpc::Server> server;
    T *handler = nullptr;
};

class ClientImpl : public DaemonRPC::ClientService::Service, public ServerHelper<ClientHandlerInterface>
{
public:
    ClientImpl(ClientHandlerInterface *handlerObject);
    grpc::Status GenerateTorrent(grpc::ServerContext *context, const DaemonRPC::GenerateTorrentRequest *request,
                                 DaemonRPC::TorrentInfo *response) override;
    grpc::Status DownloadTorrent(grpc::ServerContext *context, const DaemonRPC::TorrentInfo *request,
                                 DaemonRPC::BaseResponse *response) override;
    grpc::Status UpdateTorrentStatus(grpc::ServerContext *context, const DaemonRPC::TorrentInfo *request,
                                     DaemonRPC::BaseResponse *response) override;
};

class ClientHandlerInterface {
public:
    virtual DaemonRPC::TorrentInfo GenerateTorrent(const std::string &dir) = 0;
    virtual void DownloadTorrent(const DaemonRPC::TorrentInfo &info) = 0;
    virtual void UpdateTorrentStatus(const DaemonRPC::TorrentInfo &info) = 0;
};

/**
  * !!!!!!!!!!!    WARNING    !!!!!!!!!!!
  * This class does not used in project. GRPC server is implemented on GO language.
  * You can use this class only for some local tests.
  * No one guarantees any actual status or correct work or identical behavior with GO server.
  */
class ServerImpl : public DaemonRPC::ServerService::Service , public ServerHelper<ServerHandlerInterface>
{
public:
    ServerImpl(ServerHandlerInterface *handlerObject);

    grpc::Status HandShake(grpc::ServerContext *context,
                           const DaemonRPC::HandShakeRequest *request,
                           DaemonRPC::DaemonInfo *response) override;
    grpc::Status UpdateTorrentStatus(grpc::ServerContext *context, const DaemonRPC::UpdateTorrentStatusRequest *request,
                                     DaemonRPC::BaseResponse *response) override;
};

class ServerHandlerInterface {
public:
    virtual DaemonRPC::DaemonInfo HandShake(const std::string daemonUUID) = 0;
    virtual void UpdateTorrentStatus(const DaemonRPC::DaemonInfo &dInfo, const DaemonRPC::TorrentInfo &tInfo) = 0;
};

} //namespace grpcdaemon
#endif // GRPC_SERVER_H
