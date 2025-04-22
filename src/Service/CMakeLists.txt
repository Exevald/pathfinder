add_library(Service
        CObjLoader.cpp
        C3dsLoader.cpp
        CFileLoader.cpp
)

target_link_libraries(Service PRIVATE
        tinyobjloader
        Model
        3ds
)

target_include_directories(Service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})