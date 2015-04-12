cmake_minimum_required(VERSION 3.1)
project(task_3a_gps)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -g -O0")

set(SOURCE_FILES main.cpp)
add_executable(task_3a_gps ${SOURCE_FILES})