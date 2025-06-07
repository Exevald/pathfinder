#include "TextureMap.h"

TextureMap::TextureMap(std::unique_ptr<QOpenGLTexture> texture)
	: m_texture(std::move(texture))
	, m_sx(1.0f)
	, m_sy(1.0f)
	, m_dx(0.0f)
	, m_dy(0.0f)
	, m_rotation(0.0f)
{
}

void TextureMap::AttachTexture(std::unique_ptr<QOpenGLTexture> texture)
{
	m_texture = std::move(texture);
}

const QOpenGLTexture* TextureMap::GetTexture() const
{
	return m_texture.get();
}

bool TextureMap::IsLoaded() const
{
	return m_texture && m_texture->isCreated();
}

void TextureMap::Bind() const
{
	if (m_texture)
	{
		m_texture->bind();
	}
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
