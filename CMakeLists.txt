cmake_minimum_required(VERSION 3.20)
project(inject0l)

set(CMAKE_CXX_STANDARD 23)

add_executable(inject0l src/main.cpp src/utils/c_process.h src/utils/c_log.h src/utils/c_log.cpp src/utils/c_process.cpp)
