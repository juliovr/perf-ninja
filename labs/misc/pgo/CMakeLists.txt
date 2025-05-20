cmake_minimum_required(VERSION 3.3)

project(lab)

include_directories(lua)
file(GLOB EXT_LAB_srcs lua/*.c)
file(GLOB EXT_VALIDATE_srcs lua/*.c)

set(VALIDATE_ARGS "${CMAKE_CURRENT_SOURCE_DIR}/reference_output.txt")

# To generate the instrumented binary
#set(CMAKE_C_FLAGS "-fprofile-generate ${CMAKE_CXX_FLAGS}")
#set(CMAKE_CXX_FLAGS "-fprofile-generate ${CMAKE_CXX_FLAGS}")

# To generate the optimized executable
set(CMAKE_C_FLAGS "-fprofile-use ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "-fprofile-use ${CMAKE_CXX_FLAGS}")

string(REGEX MATCH "^(.*)[\\/]labs[\\/].*$" repo "${CMAKE_CURRENT_SOURCE_DIR}")
include(${CMAKE_MATCH_1}/tools/labs.cmake)
