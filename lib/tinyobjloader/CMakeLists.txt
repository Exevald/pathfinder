add_library(tinyobjloader
        tiny_obj_loader.cc
)

target_include_directories(tinyobjloader PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})