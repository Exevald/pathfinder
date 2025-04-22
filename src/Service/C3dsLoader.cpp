#include "C3dsLoader.h"
#include <stdexcept>
#include <vector.h>

C3dsLoader::C3dsLoader(Lib3dsFile* file)
	: m_file(file)
{
}

C3dsLoader::C3dsLoader(const char* fileName)
{
	auto file = lib3ds_file_load(fileName);
	if (!file)
	{
		throw std::runtime_error("Unable to read 3ds file");
	}
	m_file = file;
}

C3dsLoader::~C3dsLoader()
{
	if (m_file)
	{
		lib3ds_file_free(m_file);
	}
}

std::vector<CMesh> C3dsLoader::GetMeshes() const
{
	std::vector<CMesh> meshes;

	for (Lib3dsMesh* lib3dsMesh = m_file->meshes; lib3dsMesh != nullptr; lib3dsMesh = lib3dsMesh->next)
	{
		CMesh mesh;

		for (auto i = 0; i < lib3dsMesh->points; ++i)
		{
			Lib3dsPoint& point = lib3dsMesh->pointL[i];
			Lib3dsVector transformedPoint;
			lib3ds_vector_transform(transformedPoint, lib3dsMesh->matrix, point.pos);

			Vertex vertex = { { transformedPoint[0], transformedPoint[1], transformedPoint[2] } };
			mesh.AddVertex(vertex);
		}

		for (auto i = 0; i < lib3dsMesh->faces; ++i)
		{
			const Lib3dsFace& face = lib3dsMesh->faceL[i];
			Face customFace = { face.points[0], face.points[1], face.points[2] };
			mesh.AddFace(customFace);
		}

		mesh.CalculateBoundingBox();
		meshes.push_back(mesh);
	}

	lib3ds_file_free(m_file);
	return meshes;
}

C3dsLoader& C3dsLoader::operator=(const C3dsLoader& other)
{
	if (this != &other)
	{
		m_file = other.m_file;
	}
	return *this;
}

C3dsLoader& C3dsLoader::operator=(C3dsLoader&& other) noexcept
{
	if (this != &other)
	{
		m_file = other.m_file;
		other.m_file = nullptr;
	}
	return *this;
}