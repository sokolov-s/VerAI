project(verai_common CXX)
cmake_minimum_required(VERSION 2.8)

# Boost
find_package(Boost REQUIRED COMPONENTS system filesystem)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Boost_INCLUDE_DIR}
    )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g -ggdb -O0")


file(GLOB COMMON_SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB COMMON_HEADER_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

set(COMMON_INCLUDE_DIR "${COMMON_SRC_LIST}" CACHE STRING "Path to the common src files" FORCE)

add_library(${PROJECT_NAME} SHARED ${COMMON_HEADER_LIST} ${COMMON_SRC_LIST})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} pthread)
