cmake_minimum_required(VERSION 3.15)
project(example_basic_usage)

add_executable(example_basic_usage example_arithmetic.cpp)
target_include_directories(example_basic_usage PRIVATE ${CMAKE_SOURCE_DIR}/include)
