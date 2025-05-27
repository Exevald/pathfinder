add_library(Service
        ObjLoader.cpp
        ObjLoader.h
        FileLoader.cpp
        FileLoader.h
)

target_link_libraries(Service
    PRIVATE
        3D
        tinyobjloader
        3ds
        Qt6::Core
        Qt6::Gui
)

target_include_directories(Service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
)