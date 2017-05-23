project(Tests CXX)
cmake_minimum_required(VERSION 2.8)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIR}
    ${LIBTORRENT_BIN_DIR}/include
    ${PLOG_SRC_DIR}/include
    ${GRPCSERVICE_GEN_DIR}
    )

link_directories(${LIBTORRENT_BIN_DIR}/lib)

add_compile_options(-std=c++11)

set(TORRENT_GRPC_SERVER_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/grpc_server.cpp
    )

add_executable(torrent_grpc_test
    ${TORRENT_GRPC_SERVER_SRC}
    )

target_link_libraries(torrent_grpc_test
    pthread
    GRPCDaemonService
    verai_common
    )
