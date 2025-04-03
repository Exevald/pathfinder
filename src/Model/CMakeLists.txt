add_library(Model STATIC
        CGrid.cpp
        СCell.cpp
        CObstacle.cpp
        CBezierCurve.cpp
)

target_include_directories(Model PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
