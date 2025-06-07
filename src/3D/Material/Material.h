#pragma once

#include <QOpenGLShaderProgram>

class Material
{
public:
	Material();

	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetAmbient(float r, float g, float b, float a = 1.0f);
	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetShininess(float shininess);

	void Activate(QOpenGLShaderProgram* program, const QString& uniformPrefix = "material") const;

private:
	float m_shininess{};
	QVector4D m_diffuse;
	QVector4D m_specular;
	QVector4D m_ambient;
};
