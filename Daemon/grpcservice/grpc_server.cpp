#include "grpc_server.h"
#include <grpc/support/log.h>

using namespace grpcserver;

using grpc::Server;
using grpc::ServerAsyncResponseWriter;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerCompletionQueue;
using grpc::Status;
using namespace DaemonRPC;
using std::chrono::system_clock;

ClientImpl::ClientImpl(ClientHandlerInterface *handlerObject)
    : ServerHelper<DaemonRPC::ClientService::Service, ClientHandlerInterface>(handlerObject)
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
    : ServerHelper<DaemonRPC::ServerService::Service, ServerHandlerInterface>(handlerObject)
{
}

grpc::Status ServerImpl::Handshake(grpc::ServerContext */*context*/,
                                   const DaemonRPC::HandshakeRequest *request,
                                   DaemonRPC::DaemonInfo *response)
{
    *response = GetHandler()->Handshake(request->uuid());
    return grpc::Status::OK;
}

grpc::Status ServerImpl::UpdateTorrentStatus(grpc::ServerContext */*context*/, const UpdateTorrentStatusRequest *request,
                                             BaseResponse */*response*/)
{
    GetHandler()->UpdateTorrentStatus(request->agentinfo(), request->torrentinfo());
    return grpc::Status::OK;
}

grpc::Status ServerImpl::GetTaskList(grpc::ServerContext */*context*/, const DaemonInfo *request,
                                     ::grpc::ServerWriter<::DaemonRPC::Task> *writer)
{
    auto tasks = GetHandler()->GetTaskList(*request);
    for (const Task& task : tasks) {
        writer->Write(task);
    }
    return Status::OK;
}

grpc::Status ServerImpl::GetInfoForGenerateTorrents(grpc::ServerContext */*context*/, const DaemonInfo *request,
                                                    ::grpc::ServerWriter<TorrentInfo> *writer)
{
    auto torrents = GetHandler()->GetInfoForGenerateTorrents(*request);
    for (const TorrentInfo& tInfo : torrents) {
        writer->Write(tInfo);
    }
    return Status::OK;
}

grpc::Status ServerImpl::GetTorrentsForDownload(grpc::ServerContext */*context*/, const DaemonInfo *request,
                                                ::grpc::ServerWriter<TorrentInfo> *writer)
{
    auto torrents = GetHandler()->GetTorrentsForDownload(*request);
    for (const TorrentInfo& tInfo : torrents) {
        writer->Write(tInfo);
    }
    return Status::OK;
}

grpc::Status ServerImpl::GetTorrentsForUpdateStatus(grpc::ServerContext */*context*/, const DaemonInfo *request,
                                                    ::grpc::ServerWriter<TorrentInfo> *writer)
{
    auto torrents = GetHandler()->GetTorrentsForUpdateStatus(*request);
    for (const TorrentInfo& tInfo : torrents) {
        writer->Write(tInfo);
    }
    return Status::OK;
}
