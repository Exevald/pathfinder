#pragma once

#include "CVector3.h"

class CBoundingBox
{
public:
	CBoundingBox();
	CBoundingBox(const CVector3f& minCoord, const CVector3f& maxCoord);

	[[nodiscard]] bool IsEmpty() const;
	[[nodiscard]] CBoundingBox Union(const CBoundingBox& other) const;

	[[nodiscard]] const CVector3f& GetMinCoord() const;
	[[nodiscard]] const CVector3f& GetMaxCoord() const;

	[[nodiscard]] CVector3f GetSize() const;
	[[nodiscard]] CVector3f GetCenter() const;

private:
	bool m_isEmpty;
	CVector3f m_minCoord;
	CVector3f m_maxCoord;
};