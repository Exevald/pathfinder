add_library(Service
        CObjLoader.cpp
        CFileLoader.cpp
)

target_link_libraries(Service PRIVATE
        3D
        tinyobjloader
        3ds
)

target_include_directories(Service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})