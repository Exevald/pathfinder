#pragma once

#include "CBoundingBox.h"
#include "Face.h"
#include "Vertex.h"
#include <vector>

class CMesh
{
public:
	void CalculateBoundingBox();
	void AddVertex(const Vertex& vertex);
	void AddFace(const Face& face);

	[[nodiscard]] CBoundingBox const& GetBoundingBox() const;

private:
	std::vector<Vertex> m_vertices;
	std::vector<Face> m_faces;
	CBoundingBox m_boundingBox;
};