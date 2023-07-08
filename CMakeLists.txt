cmake_minimum_required(VERSION 3.22.1)
project(mini-redis-2)

#include(CTest)
#enable_testing()

set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/mini-redis-server/include)
include_directories(${CMAKE_SOURCE_DIR}/mini-redis-client/include)
add_subdirectory(mini-redis-server)
add_subdirectory(mini-redis-client)