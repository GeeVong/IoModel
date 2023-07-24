cmake_minimum_required(VERSION 3.25)
project(io_model_client)
set(CMAKE_CXX_STANDARD 17)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin)
MESSAGE(STATUS "==输出目录==:" ${PROJECT_BINARY_DIR}/../bin)



add_executable(io_model_client main.cpp)