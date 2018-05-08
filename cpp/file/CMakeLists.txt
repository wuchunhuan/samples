cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(/usr/local/include)
add_compile_options(-g -Wall -fpermissive -std=c++11)
add_library(fileio SHARED file.cpp)
add_library(fileio_s STATIC file.cpp)

set_target_properties(fileio_s PROPERTIES OUTPUT_NAME "fileio")
set_target_properties(fileio PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(fileio_s PROPERTIES CLEAN_DIRECT_OUTPUT 1)