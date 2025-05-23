#include "../src/Model/Grid.h"
#include <gtest/gtest.h>
#include <queue>
#include <unordered_set>

TEST(Grid, TestGridInitialization)
{
	int gridSizeX = 10, gridSizeY = 10, gridSizeZ = 5;
	double layerHeight = 2.0;
	double droneRadius = 0.5;
	double k = 1.0;

	Grid grid(gridSizeX, gridSizeY, gridSizeZ, layerHeight, droneRadius, k, 1);

	EXPECT_EQ(grid.GetCells().size(), gridSizeX);
	EXPECT_EQ(grid.GetCells()[0].size(), gridSizeY);
	EXPECT_EQ(grid.GetCells()[0][0].size(), gridSizeZ);

	EXPECT_EQ(grid.GetCells()[0][0][0].GetConnections().size(), 3);
	EXPECT_EQ(grid.GetCells()[1][1][1].GetConnections().size(), 6);
}

TEST(Grid, TestObstacleCostCalculation)
{
	int gridSizeX = 10, gridSizeY = 10, gridSizeZ = 5;
	double layerHeight = 2.0;
	double droneRadius = 0.5;
	double k = 1.0;

	Grid grid(gridSizeX, gridSizeY, gridSizeZ, layerHeight, droneRadius, k, 1);
	grid.AddObstacle(Obstacle(2, 4, 2, 4, 0, 2));

	EXPECT_EQ(grid.GetCells()[2][2][0].GetCost(), 254.0);
	EXPECT_EQ(grid.GetCells()[4][4][2].GetCost(), 254.0);

	EXPECT_GT(grid.GetCells()[1][2][0].GetCost(), 50.0);
	EXPECT_GT(grid.GetCells()[5][4][2].GetCost(), 50.0);

	EXPECT_EQ(grid.GetCells()[0][0][0].GetCost(), 50.0);
	EXPECT_EQ(grid.GetCells()[9][9][4].GetCost(), 50.0 + 4 * layerHeight * k);
}

TEST(Grid, TestFindPathWithSmoothTrajectory)
{
	Grid grid(10, 10, 5, 2.0, 0.5, 1.0, 1);
	grid.AddObstacle(Obstacle(2, 4, 2, 4, 0, 2));

	Cell* start = &grid.GetCells()[0][0][0];
	Cell* goal = &grid.GetCells()[9][9][4];

	std::vector<std::array<double, 4>> smoothPath = grid.FindPath(start, goal);

	EXPECT_FALSE(smoothPath.empty());
	EXPECT_EQ(smoothPath.front()[0], 0.0);
	EXPECT_EQ(smoothPath.back()[0], 9.0);
}

TEST(Grid, TestPathBlockedByObstacle)
{
	Grid grid(10, 10, 5, 2.0, 0.5, 1.0, 1);
	grid.AddObstacle(Obstacle(0, 9, 0, 9, 0, 5));

	Cell* start = &grid.GetCells()[0][0][0];
	Cell* goal = &grid.GetCells()[9][9][4];

	std::vector<std::array<double, 4>> smoothPath = grid.FindPath(start, goal);

	EXPECT_TRUE(smoothPath.empty());
}

TEST(Grid, TestPathThroughMultipleLayers)
{
	Grid grid(10, 10, 5, 2.0, 0.5, 1.0, 1);
	grid.AddObstacle(Obstacle(2, 4, 2, 4, 0, 2));

	Cell* start = &grid.GetCells()[0][0][0];
	Cell* goal = &grid.GetCells()[9][9][4];

	std::vector<std::array<double, 4>> smoothPath = grid.FindPath(start, goal);

	EXPECT_FALSE(smoothPath.empty());
	EXPECT_EQ(smoothPath.front()[2], 0.0);
	EXPECT_EQ(smoothPath.back()[2], 4.0);
}

TEST(Grid, TestSmallGrid)
{
	Grid grid(3, 3, 3, 2.0, 0.5, 1.0, 1);

	Cell* start = &grid.GetCells()[0][0][0];
	Cell* goal = &grid.GetCells()[2][2][2];

	std::vector<std::array<double, 4>> smoothPath = grid.FindPath(start, goal);

	EXPECT_FALSE(smoothPath.empty());
	EXPECT_EQ(smoothPath.front()[0], 0.0);
	EXPECT_EQ(smoothPath.back()[0], 2.0);
}

TEST(Grid, TestStartAndGoalAreSame)
{
	Grid grid(10, 10, 5, 2.0, 0.5, 1.0, 1);

	Cell* start = &grid.GetCells()[5][5][2];
	Cell* goal = &grid.GetCells()[5][5][2];

	std::vector<std::array<double, 4>> smoothPath = grid.FindPath(start, goal);

	EXPECT_TRUE(smoothPath.empty());
}

TEST(Grid, TestGoalOnBoundary)
{
	Grid grid(10, 10, 5, 2.0, 0.5, 1.0, 1);

	Cell* start = &grid.GetCells()[0][0][0];
	Cell* goal = &grid.GetCells()[9][9][4];

	std::vector<std::array<double, 4>> smoothPath = grid.FindPath(start, goal);

	EXPECT_FALSE(smoothPath.empty());
	EXPECT_EQ(smoothPath.back()[0], 9.0);
	EXPECT_EQ(smoothPath.back()[1], 9.0);
	EXPECT_EQ(smoothPath.back()[2], 4.0);
}

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}