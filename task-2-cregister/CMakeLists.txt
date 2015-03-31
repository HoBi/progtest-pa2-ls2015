cmake_minimum_required(VERSION 3.1)
project(task_2_cregister)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall -g -O0")

set(SOURCE_FILES
    main.cpp)

add_executable(task_2_cregister ${SOURCE_FILES})