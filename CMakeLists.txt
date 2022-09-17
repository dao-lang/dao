cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 14)

project(dao C CXX)

# ANTLR4
find_path(ANTLR4_INCLUDE_DIR antlr4-runtime.h PATH_SUFFIXES antlr4-runtime REQUIRED)
find_library(ANTLR4_LIBRARY NAMES antlr4-runtime REQUIRED)
include_directories(${ANTLR4_INCLUDE_DIR})

message(STATUS "Found ANTLR4: ${ANTLR4_INCLUDE_DIR} ${ANTLR4_LIBRARY}")

# DAO
include_directories(generated src)

file(GLOB_RECURSE GENERATED_FILE generated/*.cpp)
file(GLOB_RECURSE SOURCE_FILE src/*.cpp src/*.hpp)

add_executable(dao ${GENERATED_FILE} ${SOURCE_FILE})
target_link_libraries(dao ${ANTLR4_LIBRARY})