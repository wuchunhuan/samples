cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE ON)

include_directories(/usr/local/include)
add_compile_options(-g -Wall -fpermissive -std=c++11)
add_library(msgq SHARED msgq.cpp)
add_library(msgq_s STATIC msgq.cpp)

set_target_properties(msgq_s PROPERTIES OUTPUT_NAME "msgq")
set_target_properties(msgq PROPERTIES CLEAN_DIRECT_OUTPUT 1)
set_target_properties(msgq_s PROPERTIES CLEAN_DIRECT_OUTPUT 1)