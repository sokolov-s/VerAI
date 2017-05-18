#include "grpc_client.h"

#include <memory>

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::ChannelArguments;
using grpc::Status;
using DaemonRPC::DownloadRequest;
using DaemonRPC::DownloadResponse;
using DaemonRPC::DownloadStatusRequest;
using DaemonRPC::DownloadStatusResponse;
using DaemonRPC::GenerateMagnetRequest;
using DaemonRPC::GenerateMagnetResponse;
using namespace DaemonRPC;

using namespace grpcservice;

GRPCClient::GRPCClient(std::shared_ptr<Channel> channel)
    : stub(DaemonService::NewStub(channel))
{

}
