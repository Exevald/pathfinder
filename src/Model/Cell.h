#pragma once

#include <vector>

struct Coords
{
	int x;
	int y;
	int z;
};

class Cell
{
public:
	explicit Cell(Coords coords, double cellLength);

	void AddConnection(Cell* neighbor);
	void SetCost(double cost);

	[[nodiscard]] Coords GetCoords() const;
	[[nodiscard]] const std::vector<Cell*>& GetConnections() const;
	[[nodiscard]] double GetCost() const;
	[[nodiscard]] double GetCellLength() const;

private:
	Coords m_coords;
	double m_cellLength;
	double m_cost;
	std::vector<Cell*> m_connections;
};