cmake_minimum_required(VERSION 3.1)
project(task_1a_video_filter)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(task_1a_video_filter ${SOURCE_FILES})