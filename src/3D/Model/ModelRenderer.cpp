#include "ModelRenderer.h"
#include "Vertex.h"

ModelRenderer::ModelRenderer()
    : f(nullptr), m_shader(nullptr)
{
}

ModelRenderer::ModelRenderer(QOpenGLFunctions_3_2_Core* f, QOpenGLShaderProgram* shader)
    : f(f), m_shader(shader)
{
}

void ModelRenderer::RenderModel(const Model& model) const
{
    if (!f || !m_shader) return;

    const auto meshCount = static_cast<unsigned>(model.GetMeshCount());
    if (meshCount == 0) return;

    model.GetVertexBuffer().Bind();
    model.GetIndexBuffer().Bind();

    const int materialCount = static_cast<int>(model.GetMaterialCount());
    for (int material = -1; material < materialCount; ++material)
    {
        for (unsigned mesh = 0; mesh < meshCount; ++mesh)
        {
			const bool needToActivateMaterial = (material >= 0);
            RenderMaterialSubMesh(model, mesh, material, needToActivateMaterial);
        }
    }

    model.GetIndexBuffer().Bind();
    model.GetVertexBuffer().Bind();
}

void ModelRenderer::RenderMaterialSubMesh(
    const Model& model,
    unsigned meshIndex,
    int materialIndex,
    bool activateMaterial) const
{
    const Mesh& mesh = model.GetMesh(meshIndex);
    int materialSubMeshIndex = mesh.GetMaterialSubMesh(materialIndex);
    if (materialSubMeshIndex < 0) return;

    if (activateMaterial && materialIndex >= 0)
    {
        const ModelMaterial& modelMaterial = model.GetMaterial(materialIndex);
        modelMaterial.GetMaterial().Activate(m_shader);

        if (modelMaterial.HasTextureMap1())
        {
            f->glActiveTexture(GL_TEXTURE0);
            modelMaterial.GetTextureMap1().Bind();
            m_shader->setUniformValue("useTexture", 1);
            m_shader->setUniformValue("diffuseTexture", 0);
        }
        else
        {
            m_shader->setUniformValue("useTexture", 0);
        }

        if (modelMaterial.IsTwoSided())
        {
            f->glDisable(GL_CULL_FACE);
        }
        else
        {
            f->glEnable(GL_CULL_FACE);
        }
    }

    quintptr vertexBufferOffset = mesh.GetVertexBufferOffset();
    SetupMeshVertexAttributes(mesh, vertexBufferOffset);

    quintptr indexBufferOffset = mesh.GetIndexBufferOffset();
    RenderSubMeshFaces(mesh, materialSubMeshIndex, indexBufferOffset);
}

void ModelRenderer::SetupMeshVertexAttributes(const Mesh& mesh, quintptr vertexBufferOffset) const
{
    bool hasTexture = mesh.HasTextureCoords();
    int stride = hasTexture ? sizeof(TexturedVertex) : sizeof(Vertex);

    m_shader->enableAttributeArray(0);
    m_shader->setAttributeBuffer(0, GL_FLOAT, vertexBufferOffset + offsetof(Vertex, position), 3, stride);

    m_shader->enableAttributeArray(1);
    m_shader->setAttributeBuffer(1, GL_FLOAT, vertexBufferOffset + offsetof(Vertex, normal), 3, stride);

    if (hasTexture)
    {
        m_shader->enableAttributeArray(2);
        m_shader->setAttributeBuffer(2, GL_FLOAT, vertexBufferOffset + offsetof(TexturedVertex, texCoord), 2, stride);
    }
    else
    {
        m_shader->disableAttributeArray(2);
    }
}

void ModelRenderer::RenderSubMeshFaces(const Mesh& mesh, unsigned subMeshIndex, quintptr indexBufferOffset) const
{
    const Mesh::SubMesh subMesh = mesh.GetSubMesh(subMeshIndex);

    f->glDrawElements(
        mesh.GetPrimitiveType(),
        subMesh.indexCount,
        mesh.GetIndexType(),
        reinterpret_cast<const void*>(indexBufferOffset + subMesh.startIndex * mesh.GetIndexSize())
    );
}
