#pragma once
#include <GL/gl.h>
#include <cassert>

class BaseTexture
{
public:
	// Генерируем имя для текстурного объекта
	void Create()
	{
		assert(!m_texture);
		glGenTextures(1, &m_texture);
	}

	// Удаляем текстурный объект
	void Delete()
	{
		if (m_texture)
		{
			glDeleteTextures(1, &m_texture);
			m_texture = 0;
		}
	}

	// Отвязываемся от текстурного объекта и возвращаем его идентификатор
	GLuint Detach()
	{
		GLuint texture = m_texture;
		m_texture = 0;
		return texture;
	}

	// Получаем идентификатор текстурного объекта
	operator GLuint() const
	{
		return m_texture;
	}

	// Делаем объект активным
	void BindTo(GLenum target) const
	{
		assert(m_texture != 0);
		glBindTexture(target, m_texture);
	}

	BaseTexture(BaseTexture const&) = delete;
	BaseTexture& operator=(BaseTexture const&) = delete;

	virtual ~BaseTexture() = default;

protected:
	explicit BaseTexture(GLuint texture)
		: m_texture(texture)
	{
	}

	void SetTexture(GLuint texture)
	{
		m_texture = texture;
	}

private:
	GLuint m_texture;
};

template <bool t_managed, class TBase> class TextureImpl : public TBase
{
public:
	explicit TextureImpl(GLuint texture = 0)
		: TBase(texture)
	{
	}

	~TextureImpl()
	{
		bool m = t_managed;
		if (m)
		{
			TBase::Delete();
		}
	}

	// Присоединяем текстурный объект
	void Attach(GLuint texture)
	{
		if (t_managed && texture && texture != *this)
		{
			TBase::Delete();
		}
		TBase::SetTexture(texture);
	}
};

class Texture2DImpl : public BaseTexture
{
public:
	explicit Texture2DImpl(GLuint texture = 0)
		: BaseTexture(texture)
	{
	}

	void Bind() const
	{
		BindTo(GL_TEXTURE_2D);
	}

	static void TexImage(GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
	{
		glTexImage2D(GL_TEXTURE_2D, level, internalFormat, width, height, border, format, type, pixels);
	}
};

class Texture1DImpl : public BaseTexture
{
public:
	explicit Texture1DImpl(GLuint texture = 0)
		: BaseTexture(texture)
	{
	}

	void Bind() const
	{
		BindTo(GL_TEXTURE_1D);
	}

	static void TexImage(GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
	{
		glTexImage1D(GL_TEXTURE_1D, level, internalFormat, width, border, format, type, pixels);
	}
};

typedef TextureImpl<true, BaseTexture> Texture;
typedef TextureImpl<false, BaseTexture> TextureHandle;

typedef TextureImpl<true, Texture2DImpl> Texture2D;
typedef TextureImpl<false, Texture2DImpl> Texture2DHandle;

typedef TextureImpl<true, Texture1DImpl> Texture1D;
typedef TextureImpl<false, Texture1DImpl> Texture1DHandle;