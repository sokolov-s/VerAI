project(Tests CXX)
cmake_minimum_required(VERSION 2.8)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIR}
    ${LIBTORRENT_BIN_DIR}/include
    ${PLOG_SRC_DIR}/include
    ${GRPCSERVICE_GEN_DIR}
    )

link_directories(${LIBTORRENT_BIN_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -ggdb -O0")


set(TORRENT_GRPC_TEST_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/grpc_torrent_test.cpp
    )

add_executable(torrent_grpc_test
    ${TORRENT_GRPC_TEST_SRC}
    )

target_link_libraries(torrent_grpc_test
    pthread
    GRPCDaemonService
    )
