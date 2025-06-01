#include "ObjLoader.h"
#include "Model/Model.h"
#include <QImage>
#include <QOpenGLTexture>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

namespace
{
Mesh ConvertShapeToMesh(
	const tinyobj::shape_t& shape,
	const tinyobj::attrib_t& attrib)
{
	if (shape.mesh.indices.empty())
	{
		return {
			0,
			0,
			0,
			0,
			false,
			BoundingBox{},
			GL_TRIANGLES,
			GL_UNSIGNED_INT
		};
	}

	const auto& firstIdx = shape.mesh.indices[0];
	float x = attrib.vertices[3 * firstIdx.vertex_index + 0];
	float y = attrib.vertices[3 * firstIdx.vertex_index + 1];
	float z = attrib.vertices[3 * firstIdx.vertex_index + 2];
	CVector3f minCoord(x, y, z);
	CVector3f maxCoord(x, y, z);

	for (size_t i = 1; i < shape.mesh.indices.size(); ++i)
	{
		const auto& idx = shape.mesh.indices[i];
		x = attrib.vertices[3 * idx.vertex_index + 0];
		y = attrib.vertices[3 * idx.vertex_index + 1];
		z = attrib.vertices[3 * idx.vertex_index + 2];
		minCoord.x = std::min(minCoord.x, x);
		minCoord.y = std::min(minCoord.y, y);
		minCoord.z = std::min(minCoord.z, z);
		maxCoord.x = std::max(maxCoord.x, x);
		maxCoord.y = std::max(maxCoord.y, y);
		maxCoord.z = std::max(maxCoord.z, z);
	}

	const BoundingBox bbox(minCoord, maxCoord);
	std::unordered_set<unsigned> uniqueVertices;
	for (const auto& idx : shape.mesh.indices)
	{
		uniqueVertices.insert(idx.vertex_index);
	}
	const auto vertexCount = static_cast<unsigned>(uniqueVertices.size());
	const auto indexCount = static_cast<unsigned>(shape.mesh.indices.size());

	const bool hasTextureCoords = !attrib.texcoords.empty();

	Mesh mesh(
		0,
		0,
		vertexCount,
		indexCount,
		hasTextureCoords,
		bbox,
		GL_TRIANGLES,
		GL_UNSIGNED_INT);

	mesh.AddSubMesh(0, indexCount);

	return mesh;
}

std::unique_ptr<QOpenGLTexture> LoadTexture(
	const std::string& name,
	const std::string& baseFolder)
{
	QString texturePath = QString::fromStdString(baseFolder + name);
	QImage image(texturePath);
	if (image.isNull())
		throw std::runtime_error("Failed to load texture: " + texturePath.toStdString());

	auto texture = std::make_unique<QOpenGLTexture>(image.mirrored());
	texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
	texture->setMagnificationFilter(QOpenGLTexture::Linear);
	texture->setWrapMode(QOpenGLTexture::Repeat);
	return texture;
}
} // namespace

ObjLoader::ObjLoader()
	: m_isLoaded(false)
	, m_vertexBufferUsage(GL_STATIC_DRAW)
	, m_indexBufferUsage(GL_STATIC_DRAW)
	, m_ignoreMissingTextures(true)
{
}

std::vector<Mesh> ObjLoader::GetMeshes(const std::string& fileName)
{
	LoadFile(fileName);
	std::vector<Mesh> meshes;
	for (const auto& shape : m_shapes)
	{
		meshes.push_back(ConvertShapeToMesh(shape, m_attrib));
	}
	return meshes;
}

void ObjLoader::IgnoreMissingTextures(bool ignore)
{
	m_ignoreMissingTextures = ignore;
}

void ObjLoader::SetVertexBufferUsage(GLenum usage)
{
	m_vertexBufferUsage = usage;
}

void ObjLoader::SetIndexBufferUsage(GLenum usage)
{
	m_indexBufferUsage = usage;
}

void ObjLoader::LoadFile(const std::string& fileName)
{
	std::string warn, err;

	const size_t slashPos = fileName.find_last_of("/\\");
	const std::string baseFolder = (slashPos == std::string::npos)
		? ""
		: fileName.substr(0, slashPos + 1);

	if (!tinyobj::LoadObj(&m_attrib, &m_shapes, &m_materials, &warn, &err, fileName.c_str(), baseFolder.c_str()))
	{
		throw std::runtime_error(warn + err);
	}

	if (!warn.empty())
	{
		std::cout << "Warning: " << warn << std::endl;
	}

	ProcessMaterials(m_materials, m_model, baseFolder);
	ProcessShapes(m_attrib, m_shapes, m_materials, m_model);

	m_isLoaded = true;
}

void ObjLoader::ProcessMaterials(
	const std::vector<tinyobj::material_t>& materials,
	Model& model,
	const std::string& baseFolder) const
{
	for (const auto& mat : materials)
	{
		ModelMaterial& material = model.AddMaterial();
		Material& matInfo = material.GetMaterial();
		matInfo.SetAmbient(mat.ambient[0], mat.ambient[1], mat.ambient[2]);
		matInfo.SetDiffuse(mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]);
		matInfo.SetSpecular(mat.specular[0], mat.specular[1], mat.specular[2]);
		matInfo.SetShininess(mat.shininess);
		material.SetTwoSided(false);

		if (!mat.diffuse_texname.empty())
		{
			try
			{
				auto texture = LoadTexture(mat.diffuse_texname, baseFolder);
				material.AddTextureMap1(std::move(texture));
			}
			catch (const std::exception& e)
			{
				if (!m_ignoreMissingTextures)
					throw;
				std::cerr << "Failed to load texture: " << mat.diffuse_texname << std::endl;
			}
		}
	}
	if (materials.empty())
	{
		ModelMaterial& material = model.AddMaterial();
		Material& matInfo = material.GetMaterial();
		matInfo.SetAmbient(0.2f, 0.2f, 0.2f);
		matInfo.SetDiffuse(0.8f, 0.8f, 0.8f);
		matInfo.SetSpecular(0.0f, 0.0f, 0.0f);
		matInfo.SetShininess(1.0f);
	}
}

void ObjLoader::ProcessShapes(
	const tinyobj::attrib_t& attrib,
	const std::vector<tinyobj::shape_t>& shapes,
	const std::vector<tinyobj::material_t>& materials,
	Model& model) const
{
}