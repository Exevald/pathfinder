#include "Material.h"

Material::Material()
{
	SetDiffuse(1, 1, 1, 1);
	SetAmbient(0.2f, 0.2f, 0.2f, 1);
	SetSpecular(1, 1, 1, 1);
	SetShininess(0);
}

void Material::SetDiffuse(float r, float g, float b, float a)
{
	m_diffuse = QVector4D(r, g, b, a);
}

void Material::SetAmbient(float r, float g, float b, float a)
{
	m_ambient = QVector4D(r, g, b, a);
}

void Material::SetSpecular(float r, float g, float b, float a)
{
	m_specular = QVector4D(r, g, b, a);
}

void Material::SetShininess(float shininess)
{
	m_shininess = shininess;
}

void Material::Apply(QOpenGLShaderProgram* program, const QString& uniformPrefix) const
{
	program->setUniformValue((uniformPrefix + ".diffuse").toUtf8().constData(), m_diffuse);
	program->setUniformValue((uniformPrefix + ".ambient").toUtf8().constData(), m_ambient);
	program->setUniformValue((uniformPrefix + ".specular").toUtf8().constData(), m_specular);
	program->setUniformValue((uniformPrefix + ".shininess").toUtf8().constData(), m_shininess);
}
