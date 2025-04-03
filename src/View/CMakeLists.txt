add_library(View
        CWindow.cpp
)

target_link_libraries(View PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)