add_library(View
        CWindow.cpp
        COpenGLWidget.cpp
)

target_link_libraries(View PRIVATE
        Qt6::OpenGL
        Qt6::Widgets
        Qt6::Gui
        Qt6::Core
        OpenGL::GLU
        Qt6::OpenGLWidgets
)