cmake_minimum_required(VERSION 3.1)
project(task_4_cregister)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -g -O0")

set(SOURCE_FILES
    src/main.cpp)

add_executable(task_4_cregister ${SOURCE_FILES})