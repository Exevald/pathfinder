add_library(Model STATIC
        CGrid.cpp
        СCell.cpp
        CObstacle.cpp
        CBezierCurve.cpp
        CVector3.h
        Vertex.h
        CBoundingBox.cpp
        CBoundingBox.h
        CMesh.cpp
        CMesh.h
        Face.h
)

target_include_directories(Model PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
