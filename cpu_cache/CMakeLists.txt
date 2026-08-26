cmake_minimum_required(VERSION 3.15)
project(CPU_CACHE LANGUAGES CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(/utf-8)

add_executable(array_and_list array_and_list_test.cpp)
add_executable(test test.cpp)
