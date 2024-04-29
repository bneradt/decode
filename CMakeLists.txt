cmake_minimum_required(VERSION 3.12)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(test_project)
set(SOURCE_FILES decode.c)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -pedantic -Wall -Werror")

# -O0 works fine:
add_executable(decode_unoptimized ${SOURCE_FILES})
target_compile_options(decode_unoptimized PRIVATE "-O0")

# -O2 reproduces the error:
add_executable(decode_optimized ${SOURCE_FILES})
target_compile_options(decode_optimized PRIVATE "-O2")
