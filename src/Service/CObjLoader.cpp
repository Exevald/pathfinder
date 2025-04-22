#include "CObjLoader.h"
#include <iostream>
#include <stdexcept>

CObjLoader::CObjLoader(const char* fileName)
	: m_isLoaded(false)
{
	LoadFile(fileName);
}

std::vector<CMesh> CObjLoader::GetMeshes() const
{
	if (!m_isLoaded)
	{
		throw std::runtime_error("OBJ file is not loaded");
	}

	std::vector<CMesh> meshes;

	for (const auto& shape : m_shapes)
	{
		CMesh mesh;
		for (auto index : shape.mesh.indices)
		{
			Vertex vertex = {
				{ m_attrib.vertices[3 * index.vertex_index + 0],
					m_attrib.vertices[3 * index.vertex_index + 1],
					m_attrib.vertices[3 * index.vertex_index + 2] }
			};
			mesh.AddVertex(vertex);
		}

		size_t numFaces = shape.mesh.indices.size() / 3;
		for (size_t i = 0; i < numFaces; ++i)
		{
			Face face = {
				static_cast<int>(3 * i + 0),
				static_cast<int>(3 * i + 1),
				static_cast<int>(3 * i + 2)
			};
			mesh.AddFace(face);
		}

		mesh.CalculateBoundingBox();
		meshes.push_back(mesh);
	}

	return meshes;
}

void CObjLoader::LoadFile(const char* fileName)
{
	std::string warn, err;
	m_isLoaded = tinyobj::LoadObj(&m_attrib, &m_shapes, &m_materials, &warn, &err, fileName);

	if (!m_isLoaded)
	{
		throw std::runtime_error("Failed to load OBJ file: " + err);
	}
	if (!warn.empty())
	{
		std::cerr << "Warning while loading OBJ file: " << warn << std::endl;
	}
}