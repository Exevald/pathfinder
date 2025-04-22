add_library(ViewModel
        CGridViewModel.cpp
)

target_link_libraries(ViewModel Model Service)

target_include_directories(ViewModel PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})