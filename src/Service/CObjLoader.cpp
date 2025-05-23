#include "CObjLoader.h"
#include <iostream>
#include <stdexcept>

CObjLoader::CObjLoader(const char* fileName)
	: m_isLoaded(false)
{
	LoadFile(fileName);
}

std::vector<Mesh> CObjLoader::GetMeshes() const
{
	if (!m_isLoaded)
	{
		throw std::runtime_error("OBJ file is not loaded");
	}

	std::vector<Mesh> meshes;

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