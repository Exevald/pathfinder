#pragma once

#include "Model.h"
#include <QOpenGLFunctions_3_2_Core>
#include <QOpenGLShaderProgram>

class ModelRenderer
{
public:
	ModelRenderer();
	explicit ModelRenderer(QOpenGLFunctions_3_2_Core* f, QOpenGLShaderProgram* shader);

	ModelRenderer(const ModelRenderer& other) = delete;
	ModelRenderer& operator=(const ModelRenderer& other) = delete;

	void RenderModel(const Model& model) const;

private:
	void RenderMaterialSubMesh(
		const Model& model,
		unsigned meshIndex,
		int materialIndex,
		bool activateMaterial) const;

	void SetupMeshVertexAttributes(
		const Mesh& mesh,
		quintptr vertexBufferOffset) const;

	void RenderSubMeshFaces(
		const Mesh& mesh,
		unsigned subMeshIndex,
		quintptr indexBufferOffset) const;

	QOpenGLFunctions_3_2_Core* f;
	QOpenGLShaderProgram* m_shader;
};
