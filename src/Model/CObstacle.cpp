#include "CObstacle.h"

CObstacle::CObstacle(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax)
	: m_xMin(xMin)
	, m_xMax(xMax)
	, m_yMin(yMin)
	, m_yMax(yMax)
	, m_zMin(zMin)
	, m_zMax(zMax)
{
}

int CObstacle::GetXMin() const { return m_xMin; }
int CObstacle::GetXMax() const { return m_xMax; }
int CObstacle::GetYMin() const { return m_yMin; }
int CObstacle::GetYMax() const { return m_yMax; }
int CObstacle::GetZMin() const { return m_zMin; }
int CObstacle::GetZMax() const { return m_zMax; }