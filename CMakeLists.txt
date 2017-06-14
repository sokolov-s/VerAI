project(VerAI)
cmake_minimum_required(VERSION 2.8)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(LIBTORRENT_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libtorrent" CACHE STRING "Path to the libtorrent project sources" FORCE)
set(LIBTORRENT_BIN_DIR ${CMAKE_BINARY_DIR}/libtorrent CACHE STRING "Path to the libtorrent project libraries" FORCE)

set(PLOG_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/plog" CACHE STRING "Path to the plog project sources" FORCE)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(${LIBTORRENT_SRC_DIR})
add_subdirectory(Daemon)
