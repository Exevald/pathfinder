#pragma once

#include "../Vector3.h"

class BoundingBox
{
public:
	BoundingBox();
	BoundingBox(const CVector3f& minCoord, const CVector3f& maxCoord);

	[[nodiscard]] bool IsEmpty() const;
	[[nodiscard]] BoundingBox Union(const BoundingBox& other) const;

	[[nodiscard]] const CVector3f& GetMinCoord() const;
	[[nodiscard]] const CVector3f& GetMaxCoord() const;

	[[nodiscard]] CVector3f GetSize() const;
	[[nodiscard]] CVector3f GetCenter() const;

private:
	bool m_isEmpty;
	CVector3f m_minCoord;
	CVector3f m_maxCoord;
};