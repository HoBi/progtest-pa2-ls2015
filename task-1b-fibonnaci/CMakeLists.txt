cmake_minimum_required(VERSION 3.1)
project(task1a)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(task1a ${SOURCE_FILES})