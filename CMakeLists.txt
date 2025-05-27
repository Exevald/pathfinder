cmake_minimum_required(VERSION 3.27)
project(pathfinder)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

list(APPEND CMAKE_PREFIX_PATH "/usr/lib/qt6")

set(Qt6_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt6")
set(Qt6Core_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt6Core")
set(Qt6Gui_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt6Gui")
set(Qt6Widgets_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt6Widgets")
set(Qt6OpenGL_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt6OpenGL")
set(Qt6OpenGLWidgets_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt6OpenGLWidgets")

set(RESOURCES
        ${CMAKE_SOURCE_DIR}/res/Compass.svg
)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)
find_package(GTest REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
include_directories(/usr/include/lib3ds)

include_directories(${Qt6Core_INCLUDE_DIRS}
                  ${Qt6Gui_INCLUDE_DIRS}
                  ${Qt6OpenGL_INCLUDE_DIRS}
                  ${Qt6Widgets_INCLUDE_DIRS}
                  ${Qt6OpenGLWidgets_INCLUDE_DIRS})

enable_testing()

add_subdirectory(src)
add_subdirectory(test)
add_subdirectory(lib)