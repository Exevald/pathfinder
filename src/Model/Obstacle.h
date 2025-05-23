#pragma once

class Obstacle
{
public:
	Obstacle(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax);

	[[nodiscard]] int GetXMin() const;
	[[nodiscard]] int GetXMax() const;
	[[nodiscard]] int GetYMin() const;
	[[nodiscard]] int GetYMax() const;
	[[nodiscard]] int GetZMin() const;
	[[nodiscard]] int GetZMax() const;

private:
	int m_xMin;
	int m_xMax;
	int m_yMin;
	int m_yMax;
	int m_zMin;
	int m_zMax;
};