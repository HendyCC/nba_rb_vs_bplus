cmake_minimum_required(VERSION 3.16)
project(nba_tree_benchmark CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

include_directories(include)

file(GLOB SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(nba_tree_benchmark ${SOURCES})

target_link_libraries(nba_tree_benchmark PRIVATE Threads::Threads)