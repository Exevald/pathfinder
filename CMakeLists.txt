cmake_minimum_required(VERSION 3.27)
project(pathfinder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(RESOURCES
        ${CMAKE_SOURCE_DIR}/res/Compass.svg
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)
find_package(GTest REQUIRED)
include_directories(/usr/include/lib3ds)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib)