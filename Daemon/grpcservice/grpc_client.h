#ifndef GRPC_CLIENT_H
#define GRPC_CLIENT_H

#include <string>
#include <memory>
#include <grpc++/grpc++.h>

#include "daemon.grpc.pb.h"

namespace grpcservice {

class GRPCClient
{
public:
    explicit GRPCClient(std::shared_ptr<grpc::Channel> channel);

private:
    std::unique_ptr<DaemonRPC::DaemonService::Stub> stub;
};

} //namespace grpcservice

#endif // GRPC_CLIENT_H
