#include "BoundingBox.h"
#include <algorithm>
#include <stdexcept>

BoundingBox::BoundingBox()
	: m_isEmpty(false)
{
}

BoundingBox::BoundingBox(const Vector3f& minCoord, const Vector3f& maxCoord)
	: m_isEmpty(false)
	, m_minCoord(minCoord)
	, m_maxCoord(maxCoord)
{
}

bool BoundingBox::IsEmpty() const
{
	return m_isEmpty;
}

BoundingBox BoundingBox::Union(const BoundingBox& other) const
{
	if (m_isEmpty && other.m_isEmpty)
	{
		return {};
	}
	if (m_isEmpty)
	{
		return other;
	}
	if (other.m_isEmpty)
	{
		return *this;
	}
	return {
		Vector3f(
			std::min(m_minCoord.x, other.m_minCoord.x),
			std::min(m_minCoord.y, other.m_minCoord.y),
			std::min(m_minCoord.z, other.m_minCoord.z)),
		Vector3f(
			std::max(m_maxCoord.x, other.m_maxCoord.x),
			std::max(m_maxCoord.y, other.m_maxCoord.y),
			std::max(m_maxCoord.z, other.m_maxCoord.z))
	};
}

const Vector3f& BoundingBox::GetMinCoord() const
{
	if (m_isEmpty)
	{
		throw std::logic_error("Bounding box is empty");
	}
	return m_minCoord;
}

const Vector3f& BoundingBox::GetMaxCoord() const
{
	if (m_isEmpty)
	{
		throw std::logic_error("Bounding box is empty");
	}
	return m_maxCoord;
}

Vector3f BoundingBox::GetSize() const
{
	if (m_isEmpty)
	{
		return { 0, 0, 0 };
	}
	return m_maxCoord - m_minCoord;
}

Vector3f BoundingBox::GetCenter() const
{
	if (m_isEmpty)
	{
		throw std::logic_error("Bounding box is empty");
	}
	return (m_minCoord + m_maxCoord) * 0.5f;
}
