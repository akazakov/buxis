cmake_minimum_required(VERSION 3.16)
project(libiprof)
set(iprof_SRC
        hitime.hpp
        iprof.cpp
        iprof.hpp
)

add_library(iprof ${iprof_SRC})