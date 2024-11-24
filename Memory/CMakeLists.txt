cmake_minimum_required(VERSION 3.29)
project(OPESY)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(headerFiles)

set(SOURCES
    config.cpp
    console.cpp
    memory.cpp
    process.cpp
    scheduler.cpp
)

add_executable(OPESY ${SOURCES})
