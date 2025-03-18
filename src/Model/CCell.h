#pragma once

#include <vector>

struct Coords
{
	int x;
	int y;
	int z;
};

class CCell {
public:
	explicit CCell(Coords coords, double cellLength);

	void AddConnection(CCell* neighbor);
	void SetCost(double cost);

	[[nodiscard]] Coords GetCoords() const;
	[[nodiscard]] const std::vector<CCell*>& GetConnections() const;
	[[nodiscard]] double GetCost() const;
	[[nodiscard]] double GetCellLength() const;

private:
	Coords m_coords;
	double m_cellLength;
	double m_cost;
	std::vector<CCell*> m_connections;
};