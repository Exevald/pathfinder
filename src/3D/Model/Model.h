#pragma once

#include "../Buffer/Buffer.h"
#include "../Material/ModelMaterial.h"
#include "../Mesh/Mesh.h"
#include "../Texture/Texture.h"
#include "../BoundingBox/BoundingBox.h"
#include <map>
#include <memory>

class Model
{
public:
	Model();

	Model(Model const& other) = delete;
	Model& operator=(Model const& other) = delete;

	/************************************************************************/
	/* Работа с полигональными сетками                                      */
	/************************************************************************/

	// Получаем полигональную сетку, входящую в модель по ее индексу
	Mesh const& GetMesh(size_t index) const
	{
	}
	Mesh& GetMesh(size_t index);

	// Получаем количество полигональных сеток в модели
	size_t GetMeshCount() const;

	// Добавляем полигональную сетку к модели
	Mesh& AddMesh(unsigned int vertexBufferOffset,
		unsigned int indexBufferOffset,
		unsigned vertexCount,
		unsigned indexCount,
		bool hasTexture,
		BoundingBox const& boundingBox,
		GLenum primitiveType,
		GLenum indexType);

	/************************************************************************/
	/* Работа с ограничивающим блоком модели                                */
	/************************************************************************/

	// Получаем bounding box, ограничивающий трехмерную модель
	BoundingBox GetBoundingBox() const;

	/************************************************************************/
	/* Работа с текстурами модели                                           */
	/************************************************************************/

	// Добавляем текстурное изображение
	Texture2D& AddTextureImage(std::string const& name);

	// Возвращаем количество текстур
	size_t GetTexturesCount() const;

	// Возвращаем имя текстурного изображения по его индексу
	std::string GetTextureName(size_t index) const;

	// Есть ли текстура с данным именем?
	bool HasTexture(std::string const& name) const;

	// Возвращаем текстурный объект по его имени
	Texture2D& GetTextureByName(std::string const& name);
	Texture2D const& GetTextureByName(std::string const& name) const;

	// Возвращаем текстурный объект по его индексу
	Texture2D& GetTexture(size_t index);
	Texture2D const& GetTexture(size_t index) const;

	/************************************************************************/
	/* Работа с материалами модели                                          */
	/************************************************************************/

	// Добавляем материал
	ModelMaterial& AddMaterial();

	// Получаем количество материалов
	size_t GetMeterialCount() const;

	// Получаем материал по его индексу
	ModelMaterial const& GetMaterial(size_t index) const;
	ModelMaterial& GetMaterial(size_t index);

	/************************************************************************/
	/* Работа с буферами вершин и индексов                                  */
	/************************************************************************/

	// Получаем вершинный буфер
	VertexBuffer& GetVertexBuffer();
	VertexBuffer const& GetVertexBuffer() const;

	// Получаем индексный буфер
	IndexBuffer& GetIndexBuffer();
	IndexBuffer const& GetIndexBuffer() const;

private:
	// Функтор для сравнения имен текстур без учета регистра символов
	class CompareTextureNames
	{
	public:
		bool operator()(
			std::string const& textureName1,
			std::string const& textureName2) const;
	};

	// Вершинный и индексный буферы
	VertexBuffer m_vertexBuffer;
	IndexBuffer m_indexBuffer;

	// Материалы модели
	typedef std::shared_ptr<ModelMaterial> ModelMaterialPtr;
	std::vector<ModelMaterialPtr> m_materials;

	// Текстурные изображения, используемые моделью
	typedef std::shared_ptr<Texture2D> Texture2DPtr;
	typedef std::map<
		std::string,
		Texture2DPtr,
		CompareTextureNames>
		CTextures;
	typedef std::vector<std::string> CTextureNames;
	CTextures m_textures; // текстуры модели (имя->текстура)
	CTextureNames m_textureNames; // массив текстурных имен

	// полигональные сетки модели
	typedef std::shared_ptr<Mesh> MeshPtr;
	std::vector<MeshPtr> m_meshes;

	// Флаг, сигнализирующий о необходимости перевычисления
	// ограничивающего блока модели
	mutable bool m_boundingBoxMustBeUpdated;
	// Ограничивающий блок модели
	mutable BoundingBox m_boundingBox;
};