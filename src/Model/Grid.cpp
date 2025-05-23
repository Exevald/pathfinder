#include "BezierCurve.h"
#include "Grid.h"
#include <algorithm>
#include <cmath>
#include <queue>
#include <utility>

namespace
{
struct NodeWithPriority
{
	Cell* cell;
	double priority;

	bool operator<(const NodeWithPriority& other) const
	{
		return priority > other.priority;
	}
};

double EvaluateSegmentSafety(const std::vector<Cell*>& segment)
{
	double totalCost = 0.0;
	for (const Cell* cell : segment)
	{
		totalCost += cell->GetCost();
	}
	return totalCost / static_cast<double>(segment.size());
}

double CalculateSegmentTime(const double safetyScore)
{
	constexpr double baseTime = 1.0;
	constexpr double slowdownFactor = 2.0;
	return baseTime + (safetyScore - 50.0) / 254.0 * slowdownFactor;
}

std::vector<std::array<double, 4>> SmoothPath(const std::vector<Cell*>& path, double cellHeight)
{
	if (path.empty())
	{
		return {};
	}

	std::vector<std::array<double, 3>> waypoints;
	for (Cell* cell : path)
	{
		auto coords = cell->GetCoords();
		double cellLength = cell->GetCellLength();
		waypoints.push_back({ static_cast<double>(coords.x) * cellLength,
			static_cast<double>(coords.y) * cellLength,
			static_cast<double>(coords.z) * cellHeight });
	}

	std::vector<std::array<double, 3>> controlPoints;
	if (waypoints.size() < 2)
	{
		return {};
	}

	std::ranges::copy(waypoints, std::back_inserter(controlPoints));
	const BezierCurve curve(controlPoints);

	std::vector<std::array<double, 4>> smoothPathWithSpeed;
	constexpr int segmentsCount = 100;
	double totalTime = 0.0;

	for (int i = 0; i <= segmentsCount; ++i)
	{
		double t = static_cast<double>(i) / segmentsCount;
		if (i == segmentsCount)
		{
			t = 1.0;
		}
		auto point = curve.Evaluate(t);

		const size_t segmentIndex = i * (path.size() - 1) / segmentsCount;
		const double safetyScore = EvaluateSegmentSafety({ path[segmentIndex], path[segmentIndex + 1] });
		const double segmentTime = CalculateSegmentTime(safetyScore);

		totalTime += segmentTime / segmentsCount;

		smoothPathWithSpeed.push_back({ point[0], point[1], point[2], totalTime });
	}

	return smoothPathWithSpeed;
}

bool IsVerticalTransitionAllowed(const Coords& coords)
{
	return (coords.x % 5 == 0 && coords.y % 5 == 0);
}
} // namespace

Grid::Grid(const int sizeX, const int sizeY, const int sizeZ, const double layerHeight, const double droneRadius, const double k, const double cellLength)
	: m_gridSizeX(sizeX)
	, m_gridSizeY(sizeY)
	, m_gridSizeZ(sizeZ)
	, m_layerHeight(layerHeight)
	, m_droneRadius(droneRadius)
	, m_k(k)
	, m_cellLength(cellLength)
{
	m_cells.resize(
		m_gridSizeX,
		std::vector(m_gridSizeY, std::vector(m_gridSizeZ, Cell({ 0, 0, 0 }, cellLength))));

	for (int x = 0; x < m_gridSizeX; ++x)
	{
		for (int y = 0; y < m_gridSizeY; ++y)
		{
			for (int z = 0; z < m_gridSizeZ; ++z)
			{
				m_cells[x][y][z] = Cell(Coords{ x, y, z }, cellLength);
			}
		}
	}
	InitializeConnections();
}

Grid::Grid(const Grid& other)
	: m_gridSizeX(other.m_gridSizeX)
	, m_gridSizeY(other.m_gridSizeY)
	, m_gridSizeZ(other.m_gridSizeZ)
	, m_layerHeight(other.m_layerHeight)
	, m_droneRadius(other.m_droneRadius)
	, m_k(other.m_k)
	, m_cellLength(other.m_cellLength)
	, m_cells(other.m_cells)
{
}

Grid::Grid(Grid&& other) noexcept
	: m_gridSizeX(std::exchange(other.m_gridSizeX, 0))
	, m_gridSizeY(std::exchange(other.m_gridSizeY, 0))
	, m_gridSizeZ(std::exchange(other.m_gridSizeZ, 0))
	, m_layerHeight(std::exchange(other.m_layerHeight, 0.0))
	, m_droneRadius(std::exchange(other.m_droneRadius, 0.0))
	, m_k(std::exchange(other.m_k, 0.0))
	, m_cellLength(std::exchange(other.m_cellLength, 0.0))
	, m_cells(std::move(other.m_cells))
{
}

Grid& Grid::operator=(const Grid& other)
{
	if (this != &other)
	{
		Grid temp(other);
		Swap(*this, temp);
	}
	return *this;
}

Grid& Grid::operator=(Grid&& other) noexcept
{
	if (this != &other)
	{
		m_gridSizeX = std::exchange(other.m_gridSizeX, 0);
		m_gridSizeY = std::exchange(other.m_gridSizeY, 0);
		m_gridSizeZ = std::exchange(other.m_gridSizeZ, 0);
		m_layerHeight = std::exchange(other.m_layerHeight, 0.0);
		m_droneRadius = std::exchange(other.m_droneRadius, 0.0);
		m_k = std::exchange(other.m_k, 0.0);
		m_cellLength = std::exchange(other.m_cellLength, 0.0);
		m_cells = std::move(other.m_cells);
	}
	return *this;
}

void Grid::AddObstacle(const Obstacle& obstacle)
{
	m_obstacles.push_back(obstacle);
	UpdateCosts();
}

std::vector<std::array<double, 4>> Grid::FindPath(Cell* start, Cell* goal)
{
	if (!IsCellInGrid(start) || !IsCellInGrid(goal))
	{
		return {};
	}

	std::priority_queue<NodeWithPriority> openSet;
	std::unordered_set<Cell*> closedSet;
	std::unordered_map<Cell*, Cell*> cameFrom;
	std::unordered_map<Cell*, double> gScore;

	gScore[start] = 0;
	const double startF = Heuristic(*start, *goal);
	openSet.push({ start, startF });

	while (!openSet.empty())
	{
		Cell* current = openSet.top().cell;
		openSet.pop();

		if (current == goal)
		{
			std::vector<Cell*> path;
			auto currentPtr = current;
			while (currentPtr)
			{
				path.push_back(currentPtr);
				currentPtr = cameFrom[currentPtr];
			}
			std::ranges::reverse(path);
			return SmoothPath(path, m_cellLength);
		}

		closedSet.insert(current);

		for (Cell* neighbor : GetValidNeighbors(current))
		{
			if (const double tentativeG = gScore[current] + neighbor->GetCost(); !gScore.contains(neighbor) || tentativeG < gScore[neighbor])
			{
				cameFrom[neighbor] = current;
				gScore[neighbor] = tentativeG;
				double f = tentativeG + Heuristic(*neighbor, *goal);
				openSet.push({ neighbor, f });
			}
		}
	}

	return {};
}

bool Grid::IsCellInGrid(const Cell* cell) const
{
	if (!cell)
	{
		return false;
	}
	const auto [x, y, z] = cell->GetCoords();
	return x >= 0 && x < m_gridSizeX && y >= 0 && y < m_gridSizeY && z >= 0 && z < m_gridSizeZ;
}

std::vector<Cell*> Grid::GetValidNeighbors(const Cell* cell)
{
	std::vector<Cell*> neighbors;
	const auto coords = cell->GetCoords();

	for (int dx = -1; dx <= 1; ++dx)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			if (dx == 0 && dy == 0)
			{
				continue;
			}

			const int newX = coords.x + dx;
			const int newY = coords.y + dy;
			const int newZ = coords.z;

			if (newX >= 0 && newX < m_gridSizeX && newY >= 0 && newY < m_gridSizeY && newZ >= 0 && newZ < m_gridSizeZ)
			{
				if (Cell* neighbor = &m_cells[newX][newY][newZ]; neighbor->GetCost() != 254.0)
					neighbors.push_back(neighbor);
			}
		}
	}

	if (IsVerticalTransitionAllowed(coords))
	{
		for (int dz = -1; dz <= 1; dz += 2)
		{
			if (const int nz = coords.z + dz; nz >= 0 && nz < m_gridSizeZ)
			{
				if (Cell* neighbor = &m_cells[coords.x][coords.y][nz]; neighbor->GetCost() != 254.0)
					neighbors.push_back(neighbor);
			}
		}
	}

	return neighbors;
}

[[nodiscard]] std::vector<std::vector<std::vector<Cell>>>& Grid::GetCells()
{
	return m_cells;
}

std::vector<Obstacle> Grid::GetObstacles() const
{
	return m_obstacles;
}

void Grid::InitializeConnections()
{
	for (int x = 0; x < m_gridSizeX; ++x)
	{
		for (int y = 0; y < m_gridSizeY; ++y)
		{
			for (int z = 0; z < m_gridSizeZ; ++z)
			{
				if (x > 0)
					m_cells[x][y][z].AddConnection(&m_cells[x - 1][y][z]);
				if (x < m_gridSizeX - 1)
					m_cells[x][y][z].AddConnection(&m_cells[x + 1][y][z]);
				if (y > 0)
					m_cells[x][y][z].AddConnection(&m_cells[x][y - 1][z]);
				if (y < m_gridSizeY - 1)
					m_cells[x][y][z].AddConnection(&m_cells[x][y + 1][z]);
				if (z > 0)
					m_cells[x][y][z].AddConnection(&m_cells[x][y][z - 1]);
				if (z < m_gridSizeZ - 1)
					m_cells[x][y][z].AddConnection(&m_cells[x][y][z + 1]);
			}
		}
	}
}

void Grid::UpdateCosts()
{
	for (int x = 0; x < m_gridSizeX; ++x)
	{
		for (int y = 0; y < m_gridSizeY; ++y)
		{
			for (int z = 0; z < m_gridSizeZ; ++z)
			{
				m_cells[x][y][z].SetCost(CalculateCellCost(x, y, z));
			}
		}
	}
}

double Grid::CalculateCellCost(int x, int y, int z) const
{
	double minDistance = std::numeric_limits<double>::max();
	for (const auto& obstacle : m_obstacles)
	{
		if (x >= obstacle.GetXMin() && x <= obstacle.GetXMax() && y >= obstacle.GetYMin() && y <= obstacle.GetYMax() && z >= obstacle.GetZMin() && z <= obstacle.GetZMax())
		{
			return 254.0;
		}

		const double dx = std::max({ static_cast<double>(obstacle.GetXMin() - x), 0.0, static_cast<double>(x - obstacle.GetXMax()) });
		const double dy = std::max({ static_cast<double>(obstacle.GetYMin() - y), 0.0, static_cast<double>(y - obstacle.GetYMax()) });
		const double dz = std::max({ static_cast<double>(obstacle.GetZMin() - z), 0.0, static_cast<double>(z - obstacle.GetZMax()) });

		if (const double distance = std::sqrt(dx * dx + dy * dy + dz * dz); distance < minDistance)
		{
			minDistance = distance;
		}
	}
	if (minDistance == std::numeric_limits<double>::max())
	{
		return 50.0;
	}

	double cost = std::exp(-m_k * (minDistance - m_droneRadius)) * 253.0;
	cost = std::clamp(cost, 50.0, 253.0);
	cost += z * m_layerHeight * m_k;

	return cost;
}

double Grid::Heuristic(const Cell& a, const Cell& b) const
{
	const auto [aX, aY, aZ] = a.GetCoords();
	const auto [bX, bY, bZ] = b.GetCoords();
	const double dx = (aX - bX) * m_cellLength;
	const double dy = (aY - bY) * m_cellLength;
	const double dz = (aZ - bZ) * m_layerHeight;

	constexpr double fv = 2.0;
	return std::sqrt(dx * dx + dy * dy + (dz * fv) * (dz * fv));
}

void Grid::Swap(Grid& lhs, Grid& rhs) noexcept
{
	std::swap(lhs.m_gridSizeX, rhs.m_gridSizeX);
	std::swap(lhs.m_gridSizeY, rhs.m_gridSizeY);
	std::swap(lhs.m_gridSizeZ, rhs.m_gridSizeZ);
	std::swap(lhs.m_layerHeight, rhs.m_layerHeight);
	std::swap(lhs.m_droneRadius, rhs.m_droneRadius);
	std::swap(lhs.m_k, rhs.m_k);
	std::swap(lhs.m_cellLength, rhs.m_cellLength);
	std::swap(lhs.m_cells, rhs.m_cells);
}