cmake_minimum_required(VERSION 3.16)
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
message(STATUS ${ProjectId})
set(flatbufs_demo_SRC
        main.cpp
)

add_executable(flatbufs_demo ${flatbufs_demo_SRC})
target_link_libraries(flatbufs_demo flatbuffers::flatbuffers)
project(${ProjectId})