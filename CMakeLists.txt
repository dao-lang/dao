cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 14)

project(dao CXX)

# ANTLR4
find_path(ANTLR4_INCLUDE_DIR antlr4-runtime.h PATH_SUFFIXES antlr4-runtime)
find_library(ANTLR4_LIBRARY NAMES libantlr4-runtime.a antlr4-runtime REQUIRED)
include_directories(${ANTLR4_INCLUDE_DIR})

message(STATUS "Found ANTLR4: ${ANTLR4_INCLUDE_DIR} ${ANTLR4_LIBRARY}")

# LLVM
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIR})
add_definitions(${LLVM_DEFINITIONS})

message(STATUS "Found LLVM: ${LLVM_DIR} ${LLVM_INCLUDE_DIR}")

# DAO
include_directories(generated include)

file(GLOB_RECURSE GENERATED_FILE generated/*.cpp)
file(GLOB_RECURSE SOURCE_FILE src/*.cpp)

add_executable(dao ${GENERATED_FILE} ${SOURCE_FILE} include/DaoBaseLexer.hpp)
target_link_libraries(dao ${ANTLR4_LIBRARY} LLVM)