cmake_minimum_required(VERSION 3.16)
project(work_cache_size)
set(work_cache_size_SRC
        work_cache_size_main.cpp
)

add_executable(work_cache_size ${work_cache_size_SRC})
target_link_libraries(work_cache_size iprof omp)