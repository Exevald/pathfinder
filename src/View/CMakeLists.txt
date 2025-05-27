add_library(View
        Space.cpp
        Space.h
        Window.cpp
        Window.h
)

target_link_libraries(View
    PRIVATE
        ViewModel
        3D
        Qt6::OpenGL
        Qt6::Widgets
        Qt6::Gui
        Qt6::Core
        Qt6::OpenGLWidgets
        OpenGL::GLU
)

target_include_directories(View
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Qt6Core_INCLUDE_DIRS}
        ${Qt6Gui_INCLUDE_DIRS}
        ${Qt6OpenGL_INCLUDE_DIRS}
        ${Qt6Widgets_INCLUDE_DIRS}
        ${Qt6OpenGLWidgets_INCLUDE_DIRS}
)