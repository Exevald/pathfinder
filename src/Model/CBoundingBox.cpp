#include "CBoundingBox.h"

CBoundingBox::CBoundingBox()
{
}

CBoundingBox::CBoundingBox(const CVector3f& minCoord, const CVector3f& maxCoord)
{
}

bool CBoundingBox::IsEmpty() const
{
	return false;
}

CBoundingBox CBoundingBox::Union(const CBoundingBox& other) const
{
	return {};
}

const CVector3f& CBoundingBox::GetMinCoord() const
{
	return {};
}

const CVector3f& CBoundingBox::GetMaxCoord() const
{
	return {};
}

CVector3f CBoundingBox::GetSize() const
{
	return {};
}

CVector3f CBoundingBox::GetCenter() const
{
	return {};
}
