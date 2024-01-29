cmake_minimum_required(VERSION 3.16)
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
message(STATUS ${ProjectId})
set(func_call_performance_SRC
        main.cpp

)
add_executable(func_call_performance_main ${func_call_performance_SRC})
target_link_libraries(func_call_performance_main nanobench::nanobench)
project(${ProjectId})