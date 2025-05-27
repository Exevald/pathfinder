#pragma once

#include <QImage>
#include <QOpenGLContext>

class TextureLoader
{
public:
	TextureLoader();

	[[nodiscard]] GLuint LoadTexture2D(const QString& fileName, GLuint textureName = 0, GLint level = 0) const;

	void SetMinFilter(GLenum minFilter);
	void SetMagFilter(GLenum magFilter);
	void BuildMipmaps(bool buildMipmaps);
	void SetWrapMode(GLenum wrapS, GLenum wrapT);

private:
	bool m_buildMipmaps;
	GLenum m_wrapS;
	GLenum m_wrapT;
	GLenum m_minFilter;
	GLenum m_magFilter;
};
