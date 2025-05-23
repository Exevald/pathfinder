add_library(Model STATIC
        Grid.cpp
        Cell.cpp
        Obstacle.cpp
        BezierCurve.cpp
)

target_include_directories(Model PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
