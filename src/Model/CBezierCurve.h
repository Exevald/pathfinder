#pragma once

#include <array>
#include <cmath>
#include <vector>

class CBezierCurve
{
public:
	explicit CBezierCurve(const std::vector<std::array<double, 3>>& controlPoints);

	[[nodiscard]] std::array<double, 3> Evaluate(double t) const;

private:
	std::vector<std::array<double, 3>> m_controlPoints;
};
