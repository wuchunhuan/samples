cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(/usr/local/include)
add_compile_options(-g -Wall -fpermissive -std=c++11)
add_library(util SHARED util.cpp)
add_library(util_s STATIC util.cpp)

set_target_properties(util_s PROPERTIES OUTPUT_NAME "util")
set_target_properties(util PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(util_s PROPERTIES CLEAN_DIRECT_OUTPUT 1)