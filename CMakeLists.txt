cmake_minimum_required(VERSION 3.16)
project(nba_tree_benchmark CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES src/*.cpp)

add_executable(nba_tree_benchmark ${SOURCES})

add_executable(b_plus_tree_test
        tests/b_plus_tree_test.cpp
        src/b_plus_tree.cpp
)
