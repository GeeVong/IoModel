cmake_minimum_required(VERSION 3.25)
project(io_model_server)
set(CMAKE_CXX_STANDARD 17)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)
MESSAGE(STATUS "==输出目录==:" ${PROJECT_BINARY_DIR}/../bin)
include_directories(./models/spst)
include_directories(./models/spmt)
include_directories(./models/mpst)

include_directories(./models/select)
include_directories(./models/poll)
include_directories(./models/epoll)

add_executable(io_model_server ./src/main.cpp)

