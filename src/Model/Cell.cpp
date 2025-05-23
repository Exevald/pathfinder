#include "Cell.h"

Cell::Cell(Coords coords, double cellLength)
	: m_coords(coords)
	, m_cellLength(cellLength)
	, m_cost(1.0)
{
}

void Cell::AddConnection(Cell* neighbor)
{
	m_connections.push_back(neighbor);
}

void Cell::SetCost(double cost)
{
	m_cost = cost;
}

Coords Cell::GetCoords() const
{
	return m_coords;
}

const std::vector<Cell*>& Cell::GetConnections() const
{
	return m_connections;
}

double Cell::GetCost() const
{
	return m_cost;
}

double Cell::GetCellLength() const
{
	return m_cellLength;
}