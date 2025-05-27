#pragma once

#include "Material.h"
#include "Texture/TextureMap.h"
#include <memory>

class ModelMaterial
{
public:
	ModelMaterial();

	ModelMaterial(const ModelMaterial&) = delete;
	ModelMaterial& operator=(const ModelMaterial&) = delete;

	// Добавить текстурную карту №1 к материалу
	TextureMap& AddTextureMap1(std::unique_ptr<QOpenGLTexture> texture);

	// Связана ли с материалом текстурная карта №1?
	bool HasTextureMap1() const;

	// Получить текстурную карту №1
	const TextureMap& GetTextureMap1() const;
	TextureMap& GetTextureMap1();

	// Получить материал
	Material& GetMaterial();
	const Material& GetMaterial() const;

	// Материал видим с лицевой и нелицевой грани?
	bool IsTwoSided() const;
	void SetTwoSided(bool value);

private:
	TextureMap m_textureMap1;
	Material m_material;
	bool m_twoSided;
};
