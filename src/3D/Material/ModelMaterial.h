#pragma once

#include "Material.h"
#include "Texture/TextureMap.h"

class ModelMaterial
{
public:
	ModelMaterial();

	ModelMaterial(ModelMaterial const& other) = delete;
	ModelMaterial& operator=(ModelMaterial const& other) = delete;

	// Добавить текстурную карту №1 к материалу
	TextureMap& AddTextureMap1(GLuint texture);

	// Связана ли с материалом текстурная карта №1?
	[[nodiscard]] bool HasTextureMap1() const;

	// Получить текстурную карту №1
	[[nodiscard]] TextureMap const& GetTextureMap1() const;
	TextureMap& GetTextureMap1();

	// Получить материал OpenGL
	Material& GetMaterial();
	[[nodiscard]] Material const& GetMaterial() const;

	// Материал видим с лицевой и нелицевой грани?
	[[nodiscard]] bool IsTwoSided() const;
	void SetTwoSided(bool value);

private:
	TextureMap m_textureMap1;
	Material m_material;
	bool m_twoSided;
};