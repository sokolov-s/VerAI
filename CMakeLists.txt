project(VerAI)
cmake_minimum_required(VERSION 2.8)
include(ExternalProject)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(LIBTORRENT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libtorrent)

set(LIBTORRENT_BIN_DIR ${CMAKE_BINARY_DIR}/libtorrent)

ExternalProject_Add(libtorrent
    SOURCE_DIR ${LIBTORRENT_SRC_DIR}
    CONFIGURE_COMMAND cd ${LIBTORRENT_SRC_DIR} &&
        ./autotool.sh &&
        ./configure --prefix=${LIBTORRENT_BIN_DIR} --enable-logging --enable-debug --enable-dht --enable-python-binding --enable-examples=no --enable-encryption --enable-tests=yes --with-libiconv

    BUILD_COMMAND cd ${LIBTORRENT_SRC_DIR} && make
    INSTALL_COMMAND  cd ${LIBTORRENT_SRC_DIR} && make install
    )

add_subdirectory(Daemon)
