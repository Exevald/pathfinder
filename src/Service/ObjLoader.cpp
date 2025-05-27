#include "ObjLoader.h"
#include "Model/Model.h"
#include <QImage>
#include <QOpenGLTexture>
#include <iostream>
#include <stdexcept>

ObjLoader::ObjLoader()
	: m_isLoaded(false)
	, m_vertexBufferUsage(GL_STATIC_DRAW)
	, m_indexBufferUsage(GL_STATIC_DRAW)
	, m_ignoreMissingTextures(true)
{
}

std::vector<Mesh> ObjLoader::GetMeshes() const
{
	if (!m_isLoaded)
	{
		throw std::runtime_error("OBJ file is not loaded");
	}
	return {};
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

void ObjLoader::LoadFile(const std::string& fileName, Model& model) const
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	const size_t slashPos = fileName.find_last_of("/\\");
	const std::string baseFolder = (slashPos == std::string::npos)
		? ""
		: fileName.substr(0, slashPos + 1);

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, fileName.c_str(), baseFolder.c_str()))
	{
		throw std::runtime_error(warn + err);
	}

	if (!warn.empty())
	{
		std::cout << "Warning: " << warn << std::endl;
	}

	ProcessMaterials(materials, model, baseFolder);
	ProcessShapes(attrib, shapes, materials, model);
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

std::unique_ptr<QOpenGLTexture> ObjLoader::LoadTexture(
	const std::string& name,
	const std::string& baseFolder) const
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
