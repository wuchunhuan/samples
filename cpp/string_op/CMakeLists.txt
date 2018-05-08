cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(/usr/local/include)
add_compile_options(-g -Wall -fpermissive -std=c++11)
add_library(stringop SHARED string_op.cpp)
add_library(stringop_s STATIC string_op.cpp)

set_target_properties(stringop_s PROPERTIES OUTPUT_NAME "stringop")
set_target_properties(stringop PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(stringop_s PROPERTIES CLEAN_DIRECT_OUTPUT 1)