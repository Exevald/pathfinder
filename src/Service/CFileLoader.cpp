#include "CFileLoader.h"

CFileLoader::CFileLoader(std::unique_ptr<IMeshLoader> meshLoader)
	: m_meshLoader(std::move(meshLoader))
{
}

CGrid CFileLoader::LoadAndConvert()
{
	auto meshes = m_meshLoader->GetMeshes();

	CGrid grid(100, 100, 100, 1.0, 0.5, 1.0, 1.0);

	for (const auto& mesh : meshes)
	{
		const auto& boundingBox = mesh.GetBoundingBox();
		CObstacle obstacle(
			static_cast<int>(boundingBox.GetMinCoord().x),
			static_cast<int>(boundingBox.GetMaxCoord().x),
			static_cast<int>(boundingBox.GetMinCoord().y),
			static_cast<int>(boundingBox.GetMaxCoord().y),
			static_cast<int>(boundingBox.GetMinCoord().z),
			static_cast<int>(boundingBox.GetMaxCoord().z));
		grid.AddObstacle(obstacle);
	}

	return grid;
}
