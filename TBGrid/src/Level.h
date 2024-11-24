#pragma once

#include <memory>
#include <string>
#include <vector>

#include "EnemyUnit.h"
#include "GameObject.h"
#include "LevelGrid.h"
#include "Light.h"
#include "PlayerUnit.h"
#include "Material.h"
#include "Mesh.h"

//A class to hold/load the data for a level (adds level content to scene).
//For now, primarily exists to set up test environment
class Level
{
private:
	void buildCoreObjects();	//Some objects should be present in our scene for every level (e.g. markers for unit paths). We add them here
	//Functions for generating objects for test level
	void TEST_addFloorTile(int x, int y, int z, Mesh* floorMesh, Material* floorMat);
	void TEST_addSolidWall(int x, int y, int z, Mesh* wallMesh, Material* wallMat);
	void TEST_addPlayerUnit(glm::ivec3 coords);
	void TEST_addEnemyUnit(glm::ivec3 coords);
	void TEST_addLights();
	std::vector<std::shared_ptr<PlayerUnit>> playerUnits;
	std::vector<std::shared_ptr<EnemyUnit>> enemyUnits;
public:
	LevelGrid levelGrid;
	int levelWidth, levelHeight, levelDepth; //xyz axes respectively
	bool addedToScene = false;
	std::vector<std::shared_ptr<GameObject>> objects;
	std::vector<std::shared_ptr<PlayerUnit>> getActivePlayers();
	std::vector<std::shared_ptr<EnemyUnit>> getActiveEnemies();

	DirectionalLight sun;
	std::vector<PointLight> lights;
	Level();
	~Level();
	void buildTestLevel();
	void loadLevel(std::string levelName);
};

//Notes on the level grid:
/*
For now, the level is being generated on a strict grid. Walkability is on a per-grid-cell basis, unit and object locations are tied to specific grid cells
This works well for a board-game style gameplay, but I'm still considering a more free-moving style.
In a free-moving style, objects would be able to occupy only part of a cell, and we would require a proper navmesh.

For generating the set of game objects, the level grid is definitive. So when loading a level, we generate the level grid array,
then use that array to generate the actual set of gameobjects.

During gameplay, things may move around, and the level grid may have its contents modified. This means that after (but not before) the level has been first loaded into the scene
the level grid is representative of the *current* state of the level within the scene.

Note also that the level grid maintains no ownership of the objects within it. They belong to the object lists of the level and the scene.

Current issues:
1.	Because the CellInfo references the entire GameObject, it includes transform info for the object. 
	As a result, an object could be positioned well outside a cell but still be associated with that cell
*/