#include "grpc_client.h"

#include <memory>

using grpc::Channel;
using grpc::ClientAsyncResponseReader;
using grpc::ClientContext;
using grpc::CompletionQueue;
using grpc::ChannelArguments;
using grpc::Status;
using namespace DaemonRPC;

using namespace grpcserver;

GRPCClient::GRPCClient(std::shared_ptr<Channel> channel)
//    : stub(DaemonService::NewStub(channel))
{

}
