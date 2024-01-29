cmake_minimum_required(VERSION 3.16)
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId} CXX)
message(STATUS Project: ${ProjectId})

# Global packages that used everywhere
find_package(nanobench REQUIRED CONFIG)
#set(CMAKE_CXX_FLAGS_DEBUG -O)


set(CMAKE_CXX_STANDARD 20)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(func_call_performance)