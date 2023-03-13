cmake_minimum_required(VERSION 3.24)
project(state_machines)

set(CMAKE_CXX_STANDARD 20)

ADD_DEFINITIONS(-DSTAGE6)

add_subdirectory(v1)
add_subdirectory(v2)
