#pragma once

#include "../../lib/tinyobjloader/tiny_obj_loader.h"
#include "IMeshLoader.h"
#include <string>
#include <vector>

class CObjLoader final : public IMeshLoader
{
public:
	explicit CObjLoader(const char* fileName);
	[[nodiscard]] std::vector<Mesh> GetMeshes() const override;

private:
	tinyobj::attrib_t m_attrib;
	std::vector<tinyobj::shape_t> m_shapes;
	std::vector<tinyobj::material_t> m_materials;
	bool m_isLoaded;

	void LoadFile(const char* fileName);
};