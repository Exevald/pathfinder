#pragma once

#include <cmath>

class CVector3f
{
public:
	float x, y, z;

	explicit CVector3f(float x = 0.0f, float y = 0.0f, float z = 0.0f)
		: x(x)
		, y(y)
		, z(z)
	{
	}

	CVector3f operator*(float scalar) const
	{
		return CVector3f(x * scalar, y * scalar, z * scalar);
	}

	CVector3f operator-(const CVector3f& other) const
	{
		return CVector3f(x - other.x, y - other.y, z - other.z);
	}

	[[nodiscard]] CVector3f Cross(const CVector3f& other) const
	{
		return CVector3f(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x);
	}

	void Normalize()
	{
		float length = std::sqrt(x * x + y * y + z * z);
		if (length > 0.0f)
		{
			x /= length;
			y /= length;
			z /= length;
		}
	}

	explicit operator const float*() const { return &x; }
};