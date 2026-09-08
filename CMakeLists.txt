cmake_minimum_required(VERSION 3.15)

project(CPPRecipies LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_subdirectory(cpu_cache)
add_subdirectory(stl)
add_subdirectory(object_memory_layout)