add_library(pathfinder_lib STATIC
        main.cpp
        Model/CGrid.cpp
        Model/СCell.cpp
        Model/CObstacle.cpp
        Model/CBezierCurve.cpp
)

target_include_directories(pathfinder_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
