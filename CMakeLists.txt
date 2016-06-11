cmake_minimum_required(VERSION 3.5)
project(reinforcement_ipd)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp beaufils.cpp beaufils.h definitions.h)
add_executable(reinforcement_ipd ${SOURCE_FILES})