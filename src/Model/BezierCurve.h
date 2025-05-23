#pragma once

#include <array>
#include <vector>

class BezierCurve
{
public:
	explicit BezierCurve(const std::vector<std::array<double, 3>>& controlPoints);

	[[nodiscard]] std::array<double, 3> Evaluate(double t) const;

private:
	std::vector<std::array<double, 3>> m_controlPoints;
};
