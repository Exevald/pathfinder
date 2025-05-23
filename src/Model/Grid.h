#pragma once

#include "Cell.h"
#include "Obstacle.h"
#include <unordered_set>
#include <vector>

class Grid
{
public:
	Grid(int sizeX, int sizeY, int sizeZ, double layerHeight, double droneRadius, double k, double cellLength);
	Grid(const Grid& other);
	Grid(Grid&& other) noexcept;

	Grid& operator=(const Grid& other);
	Grid& operator=(Grid&& other) noexcept;

	void AddObstacle(const Obstacle& obstacle);
	std::vector<std::array<double, 4>> FindPath(Cell* start, Cell* goal);
	[[nodiscard]] std::vector<std::vector<std::vector<Cell>>>& GetCells();
	[[nodiscard]] std::vector<Obstacle> GetObstacles() const;

private:
	void InitializeConnections();
	void UpdateCosts();
	bool IsCellInGrid(const Cell* cell) const;
	[[nodiscard]] double CalculateCellCost(int x, int y, int z) const;
	std::vector<Cell*> GetValidNeighbors(const Cell* cell);
	[[nodiscard]] double Heuristic(const Cell& a, const Cell& b) const;
	static void Swap(Grid& lhs, Grid& rhs) noexcept;

	int m_gridSizeX, m_gridSizeY, m_gridSizeZ;
	double m_layerHeight;
	double m_droneRadius;
	double m_k;
	std::vector<std::vector<std::vector<Cell>>> m_cells;
	std::vector<Obstacle> m_obstacles;
	double m_cellLength;
};