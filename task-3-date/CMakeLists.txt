cmake_minimum_required(VERSION 3.1)
project(task_3_date)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -pedantic -g -O2")

set(SOURCE_FILES main.cpp)
add_executable(task_3_date ${SOURCE_FILES})