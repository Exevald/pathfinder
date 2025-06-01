#pragma once

#include "../../lib/tinyobjloader/tiny_obj_loader.h"
#include "IMeshLoader.h"
#include "Model/Model.h"
#include <QOpenGLTexture>
#include <memory>
#include <vector>

class ObjLoader final : public IMeshLoader
{
public:
	explicit ObjLoader();
	[[nodiscard]] std::vector<Mesh> GetMeshes(const std::string& fileName) override;
	void IgnoreMissingTextures(bool ignore);
	void SetVertexBufferUsage(GLenum usage);
	void SetIndexBufferUsage(GLenum usage);
	void LoadFile(const std::string& fileName);

private:
	struct MeshFace
	{
		unsigned vertices[3];
		int materialIndex;
	};

	void ProcessMaterials(
		const std::vector<tinyobj::material_t>& materials,
		Model& model,
		const std::string& baseFolder) const;

	void ProcessShapes(
		const tinyobj::attrib_t& attrib,
		const std::vector<tinyobj::shape_t>& shapes,
		const std::vector<tinyobj::material_t>& materials,
		Model& model) const;

	tinyobj::attrib_t m_attrib;
	std::vector<tinyobj::shape_t> m_shapes;
	std::vector<tinyobj::material_t> m_materials;
	Model m_model;
	bool m_isLoaded;
	GLenum m_vertexBufferUsage;
	GLenum m_indexBufferUsage;
	bool m_ignoreMissingTextures;
};
