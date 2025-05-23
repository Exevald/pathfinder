add_library(3D STATIC
        Vector3.h
        Vertex.h
        BoundingBox/BoundingBox.cpp
        Mesh/Mesh.cpp
        Mesh/Mesh.h
        Face.h
        Buffer/Buffer.h
        Buffer/BufferObject.h
        Light/DirectLight.cpp
        Light/DirectLight.h
        Light/Light.cpp
        Light/Light.h
        Material/Material.cpp
        Material/Material.h
        Model/Model.h
        Texture/Texture.h
        Texture/TextureMap.cpp
        Texture/TextureMap.h
        Rotation/RotationController.cpp
        Rotation/RotationController.h
        Material/ModelMaterial.cpp
        Material/ModelMaterial.h
)

target_include_directories(3D PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
