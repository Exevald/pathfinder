add_executable(pathfinder main.cpp)

target_link_libraries(pathfinder PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)

add_subdirectory(View)
add_subdirectory(Model)

target_link_libraries(pathfinder PRIVATE View Model)