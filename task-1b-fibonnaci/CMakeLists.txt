cmake_minimum_required(VERSION 3.1)
project(task_1b_fibonnaci)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic")

set(SOURCE_FILES main.cpp)
add_executable(task_1b_fibonnaci ${SOURCE_FILES})