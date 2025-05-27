add_library(Model STATIC
        Grid.cpp
        Grid.h
        Cell.cpp
        Cell.h
        Obstacle.cpp
        Obstacle.h
        BezierCurve.cpp
        BezierCurve.h
)

target_link_libraries(Model
    PRIVATE
        Qt6::Core
        Qt6::Gui
)

target_include_directories(Model
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
)
