cmake_minimum_required(VERSION 3.15)
project(PrimeFieldArithmetic CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Header-only library interface target
add_library(prime_field INTERFACE)
target_include_directories(prime_field INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Tests (will add later)
# enable_testing()
# add_executable(test tests/test.cpp)
# target_link_libraries(test prime_field)
# add_test(NAME ArithmeticTests COMMAND test)

# Benchmarks (will add later)
# add_executable(bench benchmarks/bench.cpp)
# target_link_libraries(bench prime_field)

# Print configuration
message(STATUS "Build configuration:")
message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Flags: ${CMAKE_CXX_FLAGS}")
