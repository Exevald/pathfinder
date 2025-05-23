#include "DirectLight.h"

DirectLight::DirectLight(CVector3f const& lightDirection)
	: m_direction(lightDirection)
{
}

void DirectLight::SetLight(const GLenum light) const
{
	const GLfloat lightDirection[4] = {
		m_direction.x,
		m_direction.y,
		m_direction.z,
		0
	};
	glLightfv(light, GL_POSITION, lightDirection);
	Light::SetLight(light);
}

void DirectLight::SetDirection(CVector3f const& direction)
{
	m_direction = direction;
}
