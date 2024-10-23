#pragma once

#include <vector>
#include "AStar.h"
#include "DebuggingTools.h"
#include "glm/glm.hpp"
#include "GameObject.h"
#include "Utilities.h"

//Describes the contents of a single cell within the level's grid
//See Level.h for more information about how the Level grid is used.
struct LevelGridCellInfo
{
	bool walkable;					//Whether the cell can be walked through under normal circumstances
	bool occupied;					//Whether a unit (or something else temporary) is in a cell, blocking pathing through it
	GameObject* geometry;			//Geometry should almost always be a staticmesh
	GameObject* additionalContents;
};

class LevelGrid
{
private:
	int width, height, depth;
	std::vector<LevelGridCellInfo> grid;
	bool pathableCell(const int loc) const;	//Private version of the pathability check that uses the internal index
public:
	LevelGrid(int width, int height, int depth);
	void setCell(const glm::ivec3 coords, LevelGridCellInfo& val);
	const LevelGridCellInfo& getCell(const glm::ivec3 coords) const;
	void addCell(LevelGridCellInfo& cell);	//Basically to only be used when constructing the grid - this assumes that appending to the grid is the location we want the cell to go
	int getCellIndexFromUnitCoords(const int x, const int y, const int z) const;
	int getCellIndexFromUnitCoords(glm::ivec3 coords) const;
	int getCellIndexFromSpatialCoords(const glm::vec3 location) const;
	glm::ivec3 getUnitCoordsFromCellIndex(const int cellIndex) const;
	glm::vec3 getSpatialCoordsFromCellIndex(const int cellIndex) const;		//returns the location of the minimal corner of the associated cell
	std::vector<int> pathBetweenTwoCells(int start, int end) const;	//Based on cell walkability, generates a route from start->end
	std::vector<int> pathBetweenPositions(glm::vec3 start, glm::vec3 end) const;	//Based on cell walkability, generates a route from start->end
	glm::ivec3 getDimensions() const;
	bool validCell(const glm::ivec3 loc) const;
	bool pathableCell(const glm::ivec3 loc) const;	//public version of the pathability check that uses the external coordinates
	void setOccupiedState(int idx, bool state);			//Tell the grid wether something is occupying a particular cell
};