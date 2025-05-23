#pragma once
#include "Light.h"
#include "Vector3.h"

class DirectLight final : public Light
{
public:
	explicit DirectLight(CVector3f const& lightDirection = CVector3f(0, 0, 1));

	void SetDirection(CVector3f const& direction);
	void SetLight(GLenum light)const override;
private:
	CVector3f m_direction;
};
