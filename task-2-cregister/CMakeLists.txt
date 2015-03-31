cmake_minimum_required(VERSION 3.1)
project(Car_Register)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pedantic -Wall")

set(SOURCE_FILES main.cpp)
add_executable(Car_Register ${SOURCE_FILES})