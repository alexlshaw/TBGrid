#pragma once

#include <memory>
#include <string>
#include <vector>

#include "BoxCollider.h"
#include "DebuggingTools.h"
#include "GameObject.h"
#include "GameObjectReference.h"
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
	void addReference(std::shared_ptr<GameObject> object, Transform transform);
public:
	bool addedToScene = false;	//Track whether we have shifted responsibility for managing the object references to the scene class
	std::vector<std::shared_ptr<GameObjectReference>> objectReferences;
	std::vector<std::shared_ptr<GameObject>> objects;
	std::vector<Light> lights;
	Level(GraphicsResourceManager* resourceManager);
	~Level();
	void buildTestLevel();
	void loadLevel(std::string levelName);
};