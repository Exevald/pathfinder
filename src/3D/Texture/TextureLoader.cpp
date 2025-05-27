#include "TextureLoader.h"

#include <QOpenGLFunctions>

TextureLoader::TextureLoader()
	: m_buildMipmaps(true),
	  m_wrapS(GL_REPEAT),
	  m_wrapT(GL_REPEAT),
	  m_minFilter(GL_LINEAR_MIPMAP_LINEAR),
	  m_magFilter(GL_LINEAR)
{
}

GLuint TextureLoader::LoadTexture2D(const QString& fileName, GLuint textureName, GLint level) const
{
	if (!QOpenGLContext::currentContext())
		throw std::runtime_error("OpenGL context is not created!");

	QImage image(fileName);
	if (image.isNull())
		throw std::runtime_error(("Error loading texture: " + fileName).toStdString());

	// Преобразуем в формат, подходящий для OpenGL
	QImage glImage = image.convertToFormat(QImage::Format_RGBA8888);

	GLuint texture = textureName;
	QOpenGLFunctions* f = QOpenGLContext::currentContext()->functions();

	if (texture == 0)
		f->glGenTextures(1, &texture);

	f->glBindTexture(GL_TEXTURE_2D, texture);

	f->glTexImage2D(
		GL_TEXTURE_2D,
		level,
		GL_RGBA,
		glImage.width(),
		glImage.height(),
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		glImage.bits()
	);

	if (m_buildMipmaps)
		f->glGenerateMipmap(GL_TEXTURE_2D);

	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_minFilter);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_magFilter);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrapS);
	f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrapT);

	return texture;
}

void TextureLoader::SetMinFilter(GLenum minFilter) { m_minFilter = minFilter; }
void TextureLoader::SetMagFilter(GLenum magFilter) { m_magFilter = magFilter; }
void TextureLoader::BuildMipmaps(bool buildMipmaps) { m_buildMipmaps = buildMipmaps; }
void TextureLoader::SetWrapMode(GLenum wrapS, GLenum wrapT) { m_wrapS = wrapS; m_wrapT = wrapT; }
