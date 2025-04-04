add_executable(pathfinder main.cpp)

add_subdirectory(Model)
add_subdirectory(View)
add_subdirectory(ViewModel)

target_link_libraries(pathfinder PRIVATE
        Model
        View
        ViewModel
        Qt6::OpenGL
        Qt6::Widgets
        Qt6::Gui
        Qt6::Core
        Qt6::OpenGLWidgets
        OpenGL::GLU
)