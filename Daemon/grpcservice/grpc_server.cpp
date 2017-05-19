#include "grpc_server.h"
#include <grpc/support/log.h>

using namespace grpcserver;

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using DaemonRPC::HandShakeRequest;
using DaemonRPC::DaemonInfo;
using DaemonRPC::TorrentInfo;
using DaemonRPC::GenerateTorrentRequest;
using DaemonRPC::UpdateTorrentStatusRequest;
using DaemonRPC::BaseResponse;
using namespace DaemonRPC;
using std::chrono::system_clock;

ClientImpl::ClientImpl(ClientHandlerInterface *handlerObject)
    : ServerHelper<ClientHandlerInterface>(handlerObject)
{
}

grpc::Status ClientImpl::GenerateTorrent(grpc::ServerContext */*context*/, const GenerateTorrentRequest *request,
                                             TorrentInfo *response)
{
    *response = GetHandler()->GenerateTorrent(request->dir());
    return grpc::Status::OK;
}

grpc::Status ClientImpl::DownloadTorrent(grpc::ServerContext */*context*/, const TorrentInfo *request,
                                             BaseResponse */*response*/)
{
    GetHandler()->DownloadTorrent(*request);
    return grpc::Status::OK;
}

grpc::Status ClientImpl::UpdateTorrentStatus(grpc::ServerContext */*context*/, const TorrentInfo *request,
                                                 BaseResponse */*response*/)
{
    GetHandler()->UpdateTorrentStatus(*request);
    return grpc::Status::OK;
}


ServerImpl::ServerImpl(ServerHandlerInterface *handlerObject)
    : ServerHelper<ServerHandlerInterface>(handlerObject)
{
}

grpc::Status ServerImpl::HandShake(grpc::ServerContext */*context*/,
                                   const DaemonRPC::HandShakeRequest *request,
                                   DaemonRPC::DaemonInfo *response)
{
    *response = GetHandler()->HandShake(request->uuid());
    return grpc::Status::OK;
}

grpc::Status ServerImpl::UpdateTorrentStatus(grpc::ServerContext */*context*/, const UpdateTorrentStatusRequest *request,
                                             BaseResponse */*response*/)
{
    GetHandler()->UpdateTorrentStatus(request->agentinfo(), request->torrentinfo());
    return grpc::Status::OK;
}
