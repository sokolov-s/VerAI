project(VerAIDaemon CXX)
cmake_minimum_required(VERSION 2.8)

# Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)

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

add_definitions(-DVERSION="1.0.0")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DLOG_LEVEL=6)
else()
    add_definitions(-DLOG_LEVEL=2)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

set(DAEMON_SRC_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daemon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/torrent.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/filesystem.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/common/uuid.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/config_daemon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/config_torrent.cpp
    )

set(DAEMON_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/daemon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/torrent.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common/noncopyable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common/filesystem.h
    ${CMAKE_CURRENT_SOURCE_DIR}/common/uuid.h
    ${CMAKE_CURRENT_SOURCE_DIR}/config/config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/config/config_daemon.h
    ${CMAKE_CURRENT_SOURCE_DIR}/config/config_torrent.h
    )

add_executable(${PROJECT_NAME}
    ${DAEMON_SRC_LIST}
    ${DAEMON_HEADER_LIST}
    )

target_link_libraries(${PROJECT_NAME}
    torrent-rasterbar
    ${Boost_LIBRARIES}
    pthread
    )

add_subdirectory(grpcservice)
add_subdirectory(tests)
