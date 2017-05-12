project(VerAIDaemon CXX)
cmake_minimum_required(VERSION 2.8)

set(DAEMON_SRC_LIST
    ./daemon.cpp
    ./main.cpp
    ./torrent.cpp
    ./common/filesystem.cpp
    ./config/config.cpp
    ./config/config_daemon.cpp
    ./config/config_torrent.cpp
    )

set(DAEMON_HEADER_LIST
    ./daemon.h
    ./torrent.h
    ./common/noncopyable.h
    ./common/filesystem.h
    ./config/config.h
    ./config/config_daemon.h
    ./config/config_torrent.h
    )


find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIR}
    ${LIBTORRENT_BIN_DIR}/include
    ${PLOG_SRC_DIR}/include
    )

link_directories(${LIBTORRENT_BIN_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Werror -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -g -ggdb -O0 -Wall")

add_definitions(-DVERSION="1.0.0")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DLOG_LEVEL=6)
else()
    add_definitions(-DLOG_LEVEL=2)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

add_executable(${PROJECT_NAME} ${DAEMON_SRC_LIST} ${DAEMON_HEADER_LIST})
target_link_libraries(${PROJECT_NAME} torrent-rasterbar ${Boost_LIBRARIES} pthread)
