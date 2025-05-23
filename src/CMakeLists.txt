add_executable(pathfinder main.cpp)

add_subdirectory(Model)
add_subdirectory(View)
add_subdirectory(ViewModel)
add_subdirectory(Service)
add_subdirectory(3D)

target_link_libraries(pathfinder PRIVATE
        Model
        View
        ViewModel
        Service
        3D
        Qt6::OpenGL
        Qt6::Widgets
        Qt6::Gui
        Qt6::Core
        Qt6::OpenGLWidgets
        OpenGL::GLU
)


foreach (res ${RESOURCES})
    add_custom_command(
            TARGET pathfinder POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${res} ${CMAKE_BINARY_DIR}/
    )
endforeach ()