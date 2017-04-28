project(VerAIDaemon)
cmake_minimum_required(VERSION 2.8)

aux_source_directory(. SRC_LIST)

find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(${CMAKE_SOURCE_DIR} ${LIBTORRENT_BIN_DIR}/include ${Boost_INCLUDE_DIR})
link_directories(${LIBTORRENT_BIN_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Werror -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -g -ggdb -O0 -Wall")

add_executable(${PROJECT_NAME} ${SRC_LIST})
target_link_libraries(${PROJECT_NAME} torrent-rasterbar ${Boost_LIBRARIES})
