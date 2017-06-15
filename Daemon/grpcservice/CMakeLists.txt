project(GRPCDaemonService CXX)
cmake_minimum_required(VERSION 2.8)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Protobuf
find_package(Protobuf REQUIRED)

# gRPC
find_package(GRPC REQUIRED)

# Proto file
set(PROTOS ${CMAKE_CURRENT_SOURCE_DIR}/protos/daemon.proto)

# Generated sources
set(PROTO_GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto-gen)
file(MAKE_DIRECTORY ${PROTO_GEN_DIR})
set(GRPCSERVICE_GEN_DIR "${PROTO_GEN_DIR}" CACHE STRING "Path to the generated files of grpcservice project" FORCE)

protobuf_generate_cpp(PROTO_SRCS PROTO_HEADER ${PROTO_GEN_DIR} ${PROTOS})
grpc_generate_cpp(GRPC_SRCS GRPC_HDRS ${PROTO_GEN_DIR} ${PROTOS})

# Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PLOG_SRC_DIR}/include
    ${PROTO_GEN_DIR}
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIR}
    ${GRPCSERVICE_GEN_DIR}
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -ggdb -O0")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DLOG_LEVEL=6)
else()
    add_definitions(-DLOG_LEVEL=2)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

set(GRPC_CLIENT_SERVER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/grpc_server.h
    ${CMAKE_CURRENT_SOURCE_DIR}/grpc_client.h
    ${CMAKE_CURRENT_SOURCE_DIR}/grpc_server.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/grpc_client.cpp
    ${PROTO_SRCS}
    ${GRPC_SRCS}
    )

add_library(${PROJECT_NAME} SHARED
    ${GRPC_CLIENT_SERVER_SRCS}
    )

target_link_libraries(${PROJECT_NAME}
    pthread
    protobuf::libprotoc
    protobuf
    gRPC::grpc++_reflection
    )
add_custom_target(generate_grpc DEPENDS ${GRPCSERVICE_GEN_DIR}/daemon.grpc.pb.cc ${GRPCSERVICE_GEN_DIR}/daemon.grpc.pb.h)
add_dependencies(${PROJECT_NAME} generate_grpc)
