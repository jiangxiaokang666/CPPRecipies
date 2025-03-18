# CMake最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(TestRecipies VERSION 1.0)

# 设置C++标准为C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 查找源文件并添加到可执行目标
add_executable(test_thread_pool ../thread/ThreadPoolWithPackagedTask.cpp ./test_thread_pool.cpp)
