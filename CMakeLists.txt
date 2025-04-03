cmake_minimum_required(VERSION 3.27)
project(pathfinder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
find_package(GTest REQUIRED)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)