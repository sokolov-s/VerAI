project(VerAI)
cmake_minimum_required(VERSION 2.8)
include(ExternalProject)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(LIBTORRENT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libtorrent" CACHE STRING "Path to the libtorrent project sources" FORCE)
set(LIBTORRENT_BIN_DIR ${CMAKE_BINARY_DIR}/libtorrent CACHE STRING "Path to the libtorrent project libraries" FORCE)

set(GRPC_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/grpc" CACHE STRING "Path to the grpc project sources" FORCE)
set(GRPC_BIN_DIR ${CMAKE_BINARY_DIR}/grpc CACHE STRING "Path to the grpc project binaries" FORCE)

set(PLOG_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/plog" CACHE STRING "Path to the plog project sources" FORCE)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)

ExternalProject_Add(grpc
    SOURCE_DIR ${GRPC_SRC_DIR}
    CONFIGURE_COMMAND cd ${GRPC_SRC_DIR} &&
        git checkout release-0_15 &&
        git submodule update --init &&
        cd ${GRPC_SRC_DIR}/grpc/third_party/protobuf &&
        ./autogen.sh &&
        ./configure

    BUILD_COMMAND cd ${GRPC_SRC_DIR}/grpc/third_party/protobuf && make
    INSTALL_COMMAND  cd ${GRPC_SRC_DIR}/grpc/third_party/protobuf && make install
    )

ExternalProject_Add(libtorrent
    SOURCE_DIR ${LIBTORRENT_SRC_DIR}
    CONFIGURE_COMMAND cd ${LIBTORRENT_SRC_DIR} &&
        git checkout libtorrent-1_1_3 &&
        ./autotool.sh &&
        ./configure --prefix=${LIBTORRENT_BIN_DIR} --enable-logging --enable-debug --disable-examples
        --enable-encryption --enable-tests=yes --with-libiconv

    BUILD_COMMAND cd ${LIBTORRENT_SRC_DIR} && make
    INSTALL_COMMAND  cd ${LIBTORRENT_SRC_DIR} && make install
    )

add_subdirectory(Daemon)
