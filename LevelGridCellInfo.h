#pragma once

#include "glm/glm.hpp"
#include "GameObject.h"

//Describes the contents of a single cell within the level's grid
//See Level.h for more information about how the Level grid is used.
struct LevelGridCellInfo
{
	bool walkable;
	GameObject* geometry;			//Geometry should almost always be a staticmesh
	GameObject* additionalContents;
};