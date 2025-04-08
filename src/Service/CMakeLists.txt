add_library(Service
        CLib3dsLoader.cpp
        CFileLoader.cpp
)

target_include_directories(Service PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})