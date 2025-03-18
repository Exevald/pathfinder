#pragma once

#include "CCell.h"
#include "CObstacle.h"
#include <cmath>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class CGrid
{
public:
	CGrid(int sizeX, int sizeY, int sizeZ, double layerHeight, double droneRadius, double k, double cellLength);

	void AddObstacle(const CObstacle& obstacle);
	std::vector<std::array<double, 4>> FindPath(CCell* start, CCell* goal);
	[[nodiscard]] std::vector<std::vector<std::vector<CCell>>>& GetCells();

private:
	void InitializeConnections();
	void UpdateCosts();
	bool IsCellInGrid(CCell* cell) const;
	double CalculateCellCost(int x, int y, int z);
	std::vector<CCell*> GetValidNeighbors(CCell* cell);
	[[nodiscard]] double Heuristic(const CCell& a, const CCell& b) const;

	int m_gridSizeX, m_gridSizeY, m_gridSizeZ;
	double m_layerHeight;
	double m_droneRadius;
	double m_k;
	std::vector<std::vector<std::vector<CCell>>> m_cells;
	std::vector<CObstacle> m_obstacles;
	double m_cellLength;
};