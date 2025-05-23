#include "ModelMaterial.h"

ModelMaterial::ModelMaterial()
	: m_twoSided(false)
{
}

Material& ModelMaterial::GetMaterial()
{
	return m_material;
}

Material const& ModelMaterial::GetMaterial() const
{
	return m_material;
}

TextureMap& ModelMaterial::AddTextureMap1(GLuint texture)
{
	m_textureMap1.AttachTexture(texture);
	return m_textureMap1;
}

bool ModelMaterial::HasTextureMap1() const
{
	return m_textureMap1.IsLoaded();
}

TextureMap const& ModelMaterial::GetTextureMap1() const
{
	return m_textureMap1;
}

TextureMap& ModelMaterial::GetTextureMap1()
{
	return m_textureMap1;
}

bool ModelMaterial::IsTwoSided() const
{
	return m_twoSided;
}

void ModelMaterial::SetTwoSided(bool value)
{
	m_twoSided = value;
}
