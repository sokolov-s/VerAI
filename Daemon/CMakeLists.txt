project(VerAIDaemon CXX)
cmake_minimum_required(VERSION 2.8)

set(DAEMON_SRC_LIST
    ./daemon.cpp
    ./main.cpp
    ./torrent.cpp
    ./config/config.cpp
    ./config/config_daemon.cpp
    ./config/config_torrent.cpp
    )

set(DAEMON_HEADER_LIST
    ./daemon.h
    ./noncopyable.h
    ./torrent.h
    ./config/config.h
    ./config/config_daemon.h
    ./config/config_torrent.h
    )


find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(${CMAKE_SOURCE_DIR} ${LIBTORRENT_BIN_DIR}/include ${Boost_INCLUDE_DIR})
link_directories(${LIBTORRENT_BIN_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Werror -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -g -ggdb -O0 -Wall")

add_definitions(-DVERSION="1.0.0")

add_executable(${PROJECT_NAME} ${DAEMON_SRC_LIST} ${DAEMON_HEADER_LIST})
target_link_libraries(${PROJECT_NAME} torrent-rasterbar ${Boost_LIBRARIES})
