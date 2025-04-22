#include "CGrid.h"
#include "CBezierCurve.h"

namespace
{
struct NodeWithPriority
{
	CCell* cell;
	double priority;

	bool operator<(const NodeWithPriority& other) const
	{
		return priority > other.priority;
	}
};

double EvaluateSegmentSafety(const std::vector<CCell*>& segment)
{
	double totalCost = 0.0;
	for (CCell* cell : segment)
	{
		totalCost += cell->GetCost();
	}
	return totalCost / static_cast<double>(segment.size());
}

double CalculateSegmentTime(double safetyScore)
{
	const double baseTime = 1.0;
	const double slowdownFactor = 2.0;
	return baseTime + (safetyScore - 50.0) / 254.0 * slowdownFactor;
}

std::vector<std::array<double, 4>> SmoothPath(const std::vector<CCell*>& path, double cellHeight)
{
	if (path.empty())
	{
		return {};
	}

	std::vector<std::array<double, 3>> waypoints;
	for (CCell* cell : path)
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

	std::copy(waypoints.begin(), waypoints.end(), std::back_inserter(controlPoints));
	CBezierCurve curve(controlPoints);

	std::vector<std::array<double, 4>> smoothPathWithSpeed;
	int segments = 100;
	double totalTime = 0.0;

	for (int i = 0; i <= segments; ++i)
	{
		double t = static_cast<double>(i) / segments;
		if (i == segments)
		{
			t = 1.0;
		}
		auto point = curve.Evaluate(t);

		size_t segmentIndex = i * (path.size() - 1) / segments;
		double safetyScore = EvaluateSegmentSafety({ path[segmentIndex], path[segmentIndex + 1] });
		double segmentTime = CalculateSegmentTime(safetyScore);

		totalTime += segmentTime / segments;

		smoothPathWithSpeed.push_back({ point[0], point[1], point[2], totalTime });
	}

	return smoothPathWithSpeed;
}

bool IsVerticalTransitionAllowed(const Coords& coords)
{
	return (coords.x % 5 == 0 && coords.y % 5 == 0);
}
} // namespace

CGrid::CGrid(int sizeX, int sizeY, int sizeZ, double layerHeight, double droneRadius, double k, double cellLength)
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
		std::vector<std::vector<CCell>>(m_gridSizeY, std::vector<CCell>(m_gridSizeZ, CCell({ 0, 0, 0 }, cellLength))));

	for (int x = 0; x < m_gridSizeX; ++x)
	{
		for (int y = 0; y < m_gridSizeY; ++y)
		{
			for (int z = 0; z < m_gridSizeZ; ++z)
			{
				m_cells[x][y][z] = CCell(Coords{ x, y, z }, cellLength);
			}
		}
	}
	InitializeConnections();
}

CGrid::CGrid(const CGrid& other)
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

CGrid::CGrid(CGrid&& other) noexcept
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

CGrid& CGrid::operator=(const CGrid& other)
{
	if (this != &other)
	{
		CGrid temp(other);
		Swap(*this, temp);
	}
	return *this;
}

CGrid& CGrid::operator=(CGrid&& other) noexcept
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

void CGrid::AddObstacle(const CObstacle& obstacle)
{
	m_obstacles.push_back(obstacle);
	UpdateCosts();
}

std::vector<std::array<double, 4>> CGrid::FindPath(CCell* start, CCell* goal)
{
	if (!IsCellInGrid(start) || !IsCellInGrid(goal))
	{
		return {};
	}

	std::priority_queue<NodeWithPriority> openSet;
	std::unordered_set<CCell*> closedSet;
	std::unordered_map<CCell*, CCell*> cameFrom;
	std::unordered_map<CCell*, double> gScore;

	gScore[start] = 0;
	double startF = Heuristic(*start, *goal);
	openSet.push({ start, startF });

	while (!openSet.empty())
	{
		CCell* current = openSet.top().cell;
		openSet.pop();

		if (current == goal)
		{
			std::vector<CCell*> path;
			auto currentPtr = current;
			while (currentPtr)
			{
				path.push_back(currentPtr);
				currentPtr = cameFrom[currentPtr];
			}
			std::reverse(path.begin(), path.end());
			return SmoothPath(path, m_cellLength);
		}

		closedSet.insert(current);

		for (CCell* neighbor : GetValidNeighbors(current))
		{
			double tentativeG = gScore[current] + neighbor->GetCost();

			if (gScore.find(neighbor) == gScore.end() || tentativeG < gScore[neighbor])
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

bool CGrid::IsCellInGrid(CCell* cell) const
{
	auto coords = cell->GetCoords();
	return coords.x >= 0 && coords.x < m_gridSizeX && coords.y >= 0 && coords.y < m_gridSizeY && coords.z >= 0 && coords.z < m_gridSizeZ;
}

std::vector<CCell*> CGrid::GetValidNeighbors(CCell* cell)
{
	std::vector<CCell*> neighbors;
	auto coords = cell->GetCoords();

	for (int dx = -1; dx <= 1; ++dx)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			if (dx == 0 && dy == 0)
			{
				continue;
			}

			int newX = coords.x + dx;
			int newY = coords.y + dy;
			int newZ = coords.z;

			if (newX >= 0 && newX < m_gridSizeX && newY >= 0 && newY < m_gridSizeY && newZ >= 0 && newZ < m_gridSizeZ)
			{
				CCell* neighbor = &m_cells[newX][newY][newZ];
				if (neighbor->GetCost() != 254.0)
					neighbors.push_back(neighbor);
			}
		}
	}

	if (IsVerticalTransitionAllowed(coords))
	{
		for (int dz = -1; dz <= 1; dz += 2)
		{
			int nz = coords.z + dz;
			if (nz >= 0 && nz < m_gridSizeZ)
			{
				CCell* neighbor = &m_cells[coords.x][coords.y][nz];
				if (neighbor->GetCost() != 254.0)
					neighbors.push_back(neighbor);
			}
		}
	}

	return neighbors;
}

[[nodiscard]] std::vector<std::vector<std::vector<CCell>>>& CGrid::GetCells()
{
	return m_cells;
}

std::vector<CObstacle> CGrid::GetObstacles() const
{
	return m_obstacles;
}

void CGrid::InitializeConnections()
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

void CGrid::UpdateCosts()
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

double CGrid::CalculateCellCost(int x, int y, int z)
{
	double minDistance = std::numeric_limits<double>::max();
	for (const auto& obstacle : m_obstacles)
	{
		if (x >= obstacle.GetXMin() && x <= obstacle.GetXMax() && y >= obstacle.GetYMin() && y <= obstacle.GetYMax() && z >= obstacle.GetZMin() && z <= obstacle.GetZMax())
		{
			return 254.0;
		}

		double dx = std::max({ (double)(obstacle.GetXMin() - x), 0.0, (double)(x - obstacle.GetXMax()) });
		double dy = std::max({ (double)(obstacle.GetYMin() - y), 0.0, (double)(y - obstacle.GetYMax()) });
		double dz = std::max({ (double)(obstacle.GetZMin() - z), 0.0, (double)(z - obstacle.GetZMax()) });
		double distance = std::sqrt(dx * dx + dy * dy + dz * dz);

		if (distance < minDistance)
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

double CGrid::Heuristic(const CCell& a, const CCell& b) const
{
	Coords aCoords = a.GetCoords();
	Coords bCoords = b.GetCoords();
	double dx = (aCoords.x - bCoords.x) * m_cellLength;
	double dy = (aCoords.y - bCoords.y) * m_cellLength;
	double dz = (aCoords.z - bCoords.z) * m_layerHeight;

	const double fv = 2.0;
	return std::sqrt(dx * dx + dy * dy + (dz * fv) * (dz * fv));
}

void CGrid::Swap(CGrid& lhs, CGrid& rhs) noexcept
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