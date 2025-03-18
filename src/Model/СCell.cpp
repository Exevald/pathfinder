#include "CCell.h"

CCell::CCell(Coords coords, double cellLength)
	: m_coords(coords)
	, m_cost(1.0)
	, m_cellLength(cellLength)
{
}

void CCell::AddConnection(CCell* neighbor)
{
	m_connections.push_back(neighbor);
}

void CCell::SetCost(double cost)
{
	m_cost = cost;
}

Coords CCell::GetCoords() const
{
	return m_coords;
}

const std::vector<CCell*>& CCell::GetConnections() const
{
	return m_connections;
}

double CCell::GetCost() const
{
	return m_cost;
}

double CCell::GetCellLength() const
{
	return m_cellLength;
}