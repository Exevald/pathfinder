#include "CBoundingBox.h"
#include <algorithm>

CBoundingBox::CBoundingBox()
	: m_isEmpty(false)
{
}

CBoundingBox::CBoundingBox(const CVector3f& minCoord, const CVector3f& maxCoord)
	: m_isEmpty(false)
	, m_minCoord(minCoord)
	, m_maxCoord(maxCoord)
{
}

bool CBoundingBox::IsEmpty() const
{
	return m_isEmpty;
}

CBoundingBox CBoundingBox::Union(const CBoundingBox& other) const
{
	if (m_isEmpty && other.m_isEmpty)
	{
		return {};
	}
	else if (m_isEmpty)
	{
		return other;
	}
	else if (other.m_isEmpty)
	{
		return *this;
	}
	else
	{
		using namespace std;
		return {
			CVector3f(
				std::min(m_minCoord.x, other.m_minCoord.x),
				std::min(m_minCoord.y, other.m_minCoord.y),
				std::min(m_minCoord.z, other.m_minCoord.z)),
			CVector3f(
				std::max(m_maxCoord.x, other.m_maxCoord.x),
				std::max(m_maxCoord.y, other.m_maxCoord.y),
				std::max(m_maxCoord.z, other.m_maxCoord.z))
		};
	}
}

const CVector3f& CBoundingBox::GetMinCoord() const
{
	if (m_isEmpty)
	{
		throw std::logic_error("Bounding box is empty");
	}
	return m_minCoord;
}

const CVector3f& CBoundingBox::GetMaxCoord() const
{
	if (m_isEmpty)
	{
		throw std::logic_error("Bounding box is empty");
	}
	return m_maxCoord;
}

CVector3f CBoundingBox::GetSize() const
{
	if (m_isEmpty)
	{
		return { 0, 0, 0 };
	}
	return m_maxCoord - m_minCoord;
}

CVector3f CBoundingBox::GetCenter() const
{
	if (m_isEmpty)
	{
		throw std::logic_error("Bounding box is empty");
	}
	return (m_minCoord + m_maxCoord) * 0.5f;
}
