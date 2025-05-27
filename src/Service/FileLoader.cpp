#include "FileLoader.h"

#include <cmath>

FileLoader::FileLoader(std::unique_ptr<IMeshLoader> meshLoader)
	: m_meshLoader(std::move(meshLoader))
{
}

Grid FileLoader::LoadAndConvert() const
{
	const auto meshes = m_meshLoader->GetMeshes();

	BoundingBox totalBoundingBox;
	for (const auto& mesh : meshes)
	{
		const auto& meshBoundingBox = mesh.GetBoundingBox();
		if (totalBoundingBox.IsEmpty())
		{
			totalBoundingBox = meshBoundingBox;
		}
		else
		{
			totalBoundingBox = totalBoundingBox.Union(meshBoundingBox);
		}
	}

	const auto& minCoord = totalBoundingBox.GetMinCoord();
	const auto& maxCoord = totalBoundingBox.GetMaxCoord();

	int gridSizeX = static_cast<int>(std::ceil(maxCoord.x - minCoord.x));
	int gridSizeY = static_cast<int>(std::ceil(maxCoord.y - minCoord.y));
	int gridSizeZ = static_cast<int>(std::ceil(maxCoord.z - minCoord.z));

	Grid grid(gridSizeX, gridSizeY, gridSizeZ, 1.0, 0.5, 1.0, 1.0);

	for (const auto& mesh : meshes)
	{
		const auto& boundingBox = mesh.GetBoundingBox();
		Obstacle obstacle(
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