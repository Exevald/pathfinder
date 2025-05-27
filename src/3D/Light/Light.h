#pragma once

#include <QOpenGLShaderProgram>
#include <QVector4D>

class Light
{
public:
	Light();

	void SetDiffuseIntensity(float r, float g, float b, float a = 1.0f);
	void SetAmbientIntensity(float r, float g, float b, float a = 1.0f);
	void SetSpecularIntensity(float r, float g, float b, float a = 1.0f);

	// Передать параметры света в шейдер
	virtual void SetUniforms(QOpenGLShaderProgram* program, const QString& uniformPrefix) const;

	virtual ~Light() = default;

protected:
	QVector4D m_diffuse;
	QVector4D m_ambient;
	QVector4D m_specular;
};
