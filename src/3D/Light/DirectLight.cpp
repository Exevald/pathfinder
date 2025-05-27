#include "DirectLight.h"
#include <QOpenGLShaderProgram>

DirectLight::DirectLight(const QVector3D& direction)
	: m_direction(direction)
{
}

void DirectLight::SetDirection(const QVector3D& direction)
{
	m_direction = direction;
}

void DirectLight::SetUniforms(QOpenGLShaderProgram* program, const QString& uniformPrefix) const
{
	Light::SetUniforms(program, uniformPrefix);
	program->setUniformValue((uniformPrefix + ".direction").toUtf8().constData(), m_direction);
}
