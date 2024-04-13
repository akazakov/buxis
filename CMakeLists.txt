cmake_minimum_required(VERSION 3.16)
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId} CXX)
message(STATUS Project: ${ProjectId})
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -pg)

# Global packages that used everywhere
find_package(nanobench REQUIRED CONFIG)
find_package(flatbuffers CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
link_libraries(fmt::fmt-header-only)
include_directories(./iprof/)

#set(CMAKE_CXX_FLAGS_DEBUG -O)


set(CMAKE_CXX_STANDARD 20)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(func_call_performance)
add_subdirectory(flatbufs_demo)
add_subdirectory(iprof)
