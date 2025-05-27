#include "Light.h"
#include <QOpenGLShaderProgram>

Light::Light()
{
	SetDiffuseIntensity(0.8f, 0.8f, 0.8f, 1.0f);
	SetAmbientIntensity(0.2f, 0.2f, 0.2f, 1.0f);
	SetSpecularIntensity(0.5f, 0.5f, 0.5f, 1.0f);
}

void Light::SetDiffuseIntensity(float r, float g, float b, float a)
{
	m_diffuse = QVector4D(r, g, b, a);
}

void Light::SetAmbientIntensity(float r, float g, float b, float a)
{
	m_ambient = QVector4D(r, g, b, a);
}

void Light::SetSpecularIntensity(float r, float g, float b, float a)
{
	m_specular = QVector4D(r, g, b, a);
}

void Light::SetUniforms(QOpenGLShaderProgram* program, const QString& uniformPrefix) const
{
	program->setUniformValue((uniformPrefix + ".diffuse").toUtf8().constData(), m_diffuse);
	program->setUniformValue((uniformPrefix + ".ambient").toUtf8().constData(), m_ambient);
	program->setUniformValue((uniformPrefix + ".specular").toUtf8().constData(), m_specular);
}
