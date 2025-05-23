#include "Obstacle.h"

Obstacle::Obstacle(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax)
	: m_xMin(xMin)
	, m_xMax(xMax)
	, m_yMin(yMin)
	, m_yMax(yMax)
	, m_zMin(zMin)
	, m_zMax(zMax)
{
}

int Obstacle::GetXMin() const { return m_xMin; }
int Obstacle::GetXMax() const { return m_xMax; }
int Obstacle::GetYMin() const { return m_yMin; }
int Obstacle::GetYMax() const { return m_yMax; }
int Obstacle::GetZMin() const { return m_zMin; }
int Obstacle::GetZMax() const { return m_zMax; }