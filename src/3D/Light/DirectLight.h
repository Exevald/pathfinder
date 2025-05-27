#pragma once

#include "Light.h"
#include <QVector3D>

class DirectLight : public Light
{
public:
	explicit DirectLight(const QVector3D& direction = QVector3D(0, 0, 1));

	void SetDirection(const QVector3D& direction);

	// Передать параметры света и направление в шейдер
	void SetUniforms(QOpenGLShaderProgram* program, const QString& uniformPrefix) const override;

private:
	QVector3D m_direction;
};
