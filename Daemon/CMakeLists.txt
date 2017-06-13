project(VerAIDaemon CXX)
cmake_minimum_required(VERSION 2.8)

# Boost
find_package(Boost REQUIRED COMPONENTS system thread filesystem)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIR}
    ${LIBTORRENT_SRC_DIR}/include
    ${PLOG_SRC_DIR}/include
    ${GRPCSERVICE_GEN_DIR}
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -ggdb -O0")

add_definitions(-DVERSION="1.0.0")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DLOG_LEVEL=6)
else()
    add_definitions(-DLOG_LEVEL=2)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

file(GLOB DAEMON_SRC_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/config/*.cpp
    )

file(GLOB DAEMON_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/config/*.h
    )

add_executable(${PROJECT_NAME}
    ${DAEMON_SRC_LIST}
    )

target_link_libraries(${PROJECT_NAME}
    torrent-rasterbar
    ${Boost_LIBRARIES}
    pthread
    verai_common
    GRPCDaemonService
    )

add_subdirectory(common)
add_subdirectory(grpcservice)
add_subdirectory(tests)
