#include "CBezierCurve.h"

CBezierCurve::CBezierCurve(const std::vector<std::array<double, 3>>& controlPoints)
	: m_controlPoints(controlPoints)
{
}

std::array<double, 3> CBezierCurve::Evaluate(double t) const
{
	std::vector<std::array<double, 3>> points = m_controlPoints;
	auto n = points.size();

	while (n > 1)
	{
		for (int i = 0; i < n - 1; ++i)
		{
			points[i][0] = (1 - t) * points[i][0] + t * points[i + 1][0];
			points[i][1] = (1 - t) * points[i][1] + t * points[i + 1][1];
			points[i][2] = (1 - t) * points[i][2] + t * points[i + 1][2];
		}
		--n;
	}

	return points[0];
}
