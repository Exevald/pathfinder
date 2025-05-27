add_library(ViewModel
        GridViewModel.cpp
        GridViewModel.h
)

target_link_libraries(ViewModel
    PRIVATE
        3D
        Model
        Service
        Qt6::Core
        Qt6::Gui
)

target_include_directories(ViewModel
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
    PRIVATE
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
)