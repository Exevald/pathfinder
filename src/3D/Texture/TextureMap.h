#pragma once

#include <QOpenGLTexture>
#include <memory>

class TextureMap
{
public:
	explicit TextureMap(std::unique_ptr<QOpenGLTexture> texture = nullptr);

	TextureMap(const TextureMap& other) = delete;
	TextureMap& operator=(const TextureMap& other) = delete;

	// Связываем текстурную карту с текстурным объектом
	void AttachTexture(std::unique_ptr<QOpenGLTexture> texture);

	// Получить текстурный объект, связанный с текстурной картой
	[[nodiscard]] const QOpenGLTexture* GetTexture() const;

	// Установить трансформацию масштабирования текстурных координат
	void SetScale(float sx, float sy);

	// Установить смещение текстурных координат
	void SetOffset(float dx, float dy);

	// Установить угол поворота текстурных координат
	void SetRotation(float rotation);

	// Связан ли текстурный объект с данной текстурной картой
	[[nodiscard]] bool IsLoaded() const;

	void Bind() const;

private:
	std::unique_ptr<QOpenGLTexture> m_texture;
	float m_sx;
	float m_sy;
	float m_dx;
	float m_dy;
	float m_rotation;
};
