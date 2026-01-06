cmake_minimum_required(VERSION 3.15)
project(PrimeFieldArithmetic CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if(DEFINED ENV{CI})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -Wall -Wextra")  # Less aggressive for CI
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra -Werror")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Header-only library interface target
add_library(prime_field INTERFACE)
target_include_directories(prime_field INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# List of all primes
set(PRIMES p64_0 p64_1 p128_0 p128_1 p192_0 p192_1 p256_0 p256_1 p512_0 p512_1)

# Create static libraries for each prime
foreach(PRIME ${PRIMES})
    add_library(prime_field_${PRIME} STATIC src/primes/${PRIME}.cpp)
    target_include_directories(prime_field_${PRIME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    )
    target_compile_features(prime_field_${PRIME} PUBLIC cxx_std_17)
endforeach()

# Create lib directory and copy static libraries after build
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
foreach(PRIME ${PRIMES})
    add_custom_command(TARGET prime_field_${PRIME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:prime_field_${PRIME}>
        ${CMAKE_SOURCE_DIR}/lib/libprime_field_${PRIME}.a
    )
endforeach()

# Enable testing
enable_testing()

# Create test executables for each prime (linking against static libs)
foreach(PRIME ${PRIMES})
    string(TOUPPER ${PRIME} PRIME_UPPER)
    add_executable(test_${PRIME} tests/tests.cpp)
    target_link_libraries(test_${PRIME} prime_field_${PRIME})
    target_compile_definitions(test_${PRIME} PRIVATE PRIME_TYPE=${PRIME_UPPER})
    add_test(NAME test_${PRIME} COMMAND test_${PRIME})
    set_tests_properties(test_${PRIME} PROPERTIES TIMEOUT 120)  # 30 second timeout per test
endforeach()

# Create benchmark executables for each prime (linking against static libs)
foreach(PRIME ${PRIMES})
    string(TOUPPER ${PRIME} PRIME_UPPER)
    add_executable(bench_${PRIME} benchmarks/bench.cpp)
    target_link_libraries(bench_${PRIME} prime_field_${PRIME})
    target_compile_definitions(bench_${PRIME} PRIVATE PRIME_TYPE=${PRIME_UPPER})
endforeach()

# Print configuration
message(STATUS "Build configuration:")
message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Flags: ${CMAKE_CXX_FLAGS}")
