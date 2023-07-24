cmake_minimum_required(VERSION 3.25)
project(iomodel)

set(CMAKE_CXX_STANDARD 17)
include_directories(../poll)
include_directories(../select)
add_executable(iomodel main.cpp)
