#include "LevelGrid.h"

LevelGrid::LevelGrid(int width, int height, int depth)
	:	width(width),
		height(height),
		depth(depth)
{
	grid.reserve(static_cast<size_t>(width * height * depth));
}

void LevelGrid::setCell(const glm::ivec3 coords, LevelGridCellInfo& val)
{
	int idx = arrayIndexFromCoords3D(coords, width, height, depth);
	grid[idx] = val;
}

const LevelGridCellInfo& LevelGrid::getCell(const glm::ivec3 coords) const
{
	int idx = arrayIndexFromCoords3D(coords, width, height, depth);
	return grid[idx];
}

void LevelGrid::addCell(LevelGridCellInfo& cell)
{
	grid.push_back(cell);
}

glm::ivec3 LevelGrid::getUnitCoordsFromCellIndex(const int cellIndex) const
{
	return arrayCoordsFromIndex3D(cellIndex, width, height, depth);
}

glm::vec3 LevelGrid::getSpatialCoordsFromCellIndex(const int cellIndex) const
{
	glm::ivec3 coords = getUnitCoordsFromCellIndex(cellIndex);
	return glm::vec3(static_cast<float>(coords.x), static_cast<float>(coords.y), static_cast<float>(coords.z));
}

int LevelGrid::getCellIndexFromUnitCoords(const int x, const int y, const int z) const
{
	return arrayIndexFromCoords3D({ x, y, z }, width, height, depth);
}

int LevelGrid::getCellIndexFromUnitCoords(glm::ivec3 coords) const
{
	return arrayIndexFromCoords3D(coords, width, height, depth);
}

int LevelGrid::getCellIndexFromSpatialCoords(const glm::vec3 location) const
{
	//Because our cells are 1 unit wide, we can just truncate each axis value
	int x = static_cast<int>(location.x);
	int y = static_cast<int>(location.y);
	int z = static_cast<int>(location.z);
	return getCellIndexFromUnitCoords(x, y, z);
}

std::vector<int> LevelGrid::pathBetweenTwoCells(int start, int end) const
{
	std::vector<int> path;
	//Quick sanity check
	if (grid[start].walkable && grid[end].walkable)
	{
		glm::ivec3 startCoords = getUnitCoordsFromCellIndex(start);
		glm::ivec3 endCoords = getUnitCoordsFromCellIndex(end);
		std::vector<AI::Node> nodePath = AI::aStar(*this, AI::Node(startCoords.x, startCoords.y, startCoords.z), AI::Node(endCoords.x, endCoords.y, endCoords.z));
		if (nodePath.size() == 0)
		{
			DEBUG_PRINTLN(std::format("Failed to generate a nav path from ({}, {}, {}) to ({}, {}, {}): no route found.", startCoords.x, startCoords.y, startCoords.z, endCoords.x, endCoords.y, endCoords.z));
		}
		//Convert nodes to indices
		for (size_t i = 0; i < nodePath.size(); i++)
		{
			//DEBUG_PRINTLN(std::format("Nav path adding node: ({}, {}, {})", nodePath[i].x, nodePath[i].y, nodePath[i].z));
			int idx = getCellIndexFromUnitCoords(nodePath[i].x, nodePath[i].y, nodePath[i].z);
			path.push_back(idx);
		}
	}
	else
	{
		DEBUG_PRINTLN("Attempting to generate nav path to/from impossible location");
	}
	return path;
}

std::vector<int> LevelGrid::pathBetweenPositions(glm::vec3 start, glm::vec3 end) const
{
	int startIdx = getCellIndexFromSpatialCoords(start);
	int endIdx = getCellIndexFromSpatialCoords(end);
	return pathBetweenTwoCells(startIdx, endIdx);
}

glm::ivec3 LevelGrid::getDimensions() const
{
	return glm::ivec3{ width, height, depth };
}

bool LevelGrid::validCell(const glm::ivec3 loc) const
{
	return loc.x >= 0 && loc.x < width && loc.y >= 0 && loc.y < height && loc.z >= 0 && loc.z < depth;
}
