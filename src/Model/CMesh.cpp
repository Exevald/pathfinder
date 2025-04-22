#include "CMesh.h"
#include <limits>

void CMesh::CalculateBoundingBox()
{
	if (m_vertices.empty())
	{
		m_boundingBox = CBoundingBox();
		return;
	}

	CVector3f minCoord = { std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
	CVector3f maxCoord = { std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest() };

	for (const auto& vertex : m_vertices)
	{
		minCoord.x = std::min(minCoord.x, vertex.position.x);
		minCoord.y = std::min(minCoord.y, vertex.position.y);
		minCoord.z = std::min(minCoord.z, vertex.position.z);

		maxCoord.x = std::max(maxCoord.x, vertex.position.x);
		maxCoord.y = std::max(maxCoord.y, vertex.position.y);
		maxCoord.z = std::max(maxCoord.z, vertex.position.z);
	}

	m_boundingBox = CBoundingBox(minCoord, maxCoord);
}

void CMesh::AddVertex(const Vertex& vertex)
{
	m_vertices.emplace_back(vertex);
}

void CMesh::AddFace(const Face& face)
{
	m_faces.emplace_back(face);
}

CBoundingBox const& CMesh::GetBoundingBox() const
{
	return m_boundingBox;
}
