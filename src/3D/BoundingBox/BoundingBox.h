#pragma once

#include "../Vector3.h"

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(const Vector3f& minCoord, const Vector3f& maxCoord);

	[[nodiscard]] bool IsEmpty() const;
	[[nodiscard]] BoundingBox Union(const BoundingBox& other) const;

	[[nodiscard]] const Vector3f& GetMinCoord() const;
	[[nodiscard]] const Vector3f& GetMaxCoord() const;

	[[nodiscard]] Vector3f GetSize() const;
	[[nodiscard]] Vector3f GetCenter() const;

private:
	bool m_isEmpty;
	Vector3f m_minCoord;
	Vector3f m_maxCoord;
};