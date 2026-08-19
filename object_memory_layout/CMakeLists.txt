cmake_minimum_required(VERSION 3.15)
project(object_memory_layout LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(/utf-8)

add_executable(player player.cpp player.h pch.h)
add_executable(vtable_example vtable_example.cpp pch.h)
add_executable(multi_object_layout multi_object_layout.cpp pch.h)
add_executable(align_example align_example.cpp pch.h)
add_executable(multi_inheritance multi_inheritance.cpp)
add_executable(virtual_call_in_construct virtual_func_in_construct.cpp)
add_executable(pure_destruct pure_destruct.cpp)
add_executable(placement_new placement_new.cpp)
add_executable(object_pool_test object_pool_test.cpp)
