#pragma once

#include <memory>
#include <string>
#include <vector>

#include "BoxCollider.h"
#include "DebuggingTools.h"
#include "GameObject.h"
#include "GraphicsResourceManager.h"
#include "Light.h"
#include "LineRenderer.h"
#include "PlayerUnit.h"
#include "Material.h"
#include "MeshTools.h"
#include "StaticMesh.h"
#include "Transform.h"

//A class to hold/load the data for a level (adds level content to scene).
//For now, primarily exists to set up test environment
class Level
{
private:
	GraphicsResourceManager* resourceManager;
	void buildCoreObjects();	//Some objects should be present in our scene for every level (e.g. markers for unit paths). We add them here
public:
	bool addedToScene = false;
	std::vector<std::shared_ptr<GameObject>> objects;
	std::vector<Light> lights;
	Level(GraphicsResourceManager* resourceManager);
	~Level();
	void buildTestLevel();
	void loadLevel(std::string levelName);
};