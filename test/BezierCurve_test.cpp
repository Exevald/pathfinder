#include "Model/CBezierCurve.h"
#include <gtest/gtest.h>

TEST(BezierCurve, TestEvaluateAtEndpoints)
{
	std::vector<std::array<double, 3>> controlPoints = {
		{ 0.0, 0.0, 0.0 },
		{ 5.0, 5.0, 5.0 },
		{ 10.0, 10.0, 10.0 }
	};

	CBezierCurve curve(controlPoints);

	auto start = curve.Evaluate(0.0);
	EXPECT_DOUBLE_EQ(start[0], 0.0);
	EXPECT_DOUBLE_EQ(start[1], 0.0);
	EXPECT_DOUBLE_EQ(start[2], 0.0);

	auto end = curve.Evaluate(1.0);
	EXPECT_DOUBLE_EQ(end[0], 10.0);
	EXPECT_DOUBLE_EQ(end[1], 10.0);
	EXPECT_DOUBLE_EQ(end[2], 10.0);
}
