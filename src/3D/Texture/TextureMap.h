#pragma once
#include "Texture.h"

class TextureMap
{
public:
	explicit TextureMap(GLuint textureId = 0);

	TextureMap(const TextureMap& other) = delete;
	TextureMap& operator=(const TextureMap& other) = delete;

	// Связываем текстурную карту с текстурным объектом OpenGL
	void AttachTexture(GLuint textureId);

	// Получить текстурный объект, связанный с текстурной картой
	[[nodiscard]] Texture2DHandle const& GetTexture() const;

	// Установить трансформацию масштабирования текстурных координат
	void SetScale(float sx, float sy);

	// Установить смещение текстурных координат
	void SetOffset(float dx, float dy);

	// Установить угол поворота текстурных координат
	void SetRotation(float rotation);

	// Связан ли текстурный объект с данонй текстурной картой
	[[nodiscard]] bool IsLoaded() const;

private:
	Texture2DHandle m_texture;
	float m_sx;
	float m_sy;
	float m_dx;
	float m_dy;
	float m_rotation;
};