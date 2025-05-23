#include "TextureMap.h"

TextureMap::TextureMap(GLuint textureId)
	: m_texture(textureId)
	  , m_sx(1)
	  , m_sy(1)
	  , m_dx(0)
	  , m_dy(0)
	  , m_rotation(0)
{
}

Texture2DHandle const& TextureMap::GetTexture() const
{
	return m_texture;
}

bool TextureMap::IsLoaded() const
{
	return m_texture != 0;
}

void TextureMap::AttachTexture(GLuint textureId)
{
	m_texture.Attach(textureId);
}

void TextureMap::SetScale(float sx, float sy)
{
	m_sx = sx;
	m_sy = sy;
}

void TextureMap::SetOffset(float dx, float dy)
{
	m_dx = dx;
	m_dy = dy;
}

void TextureMap::SetRotation(float rotation)
{
	m_rotation = rotation;
}

