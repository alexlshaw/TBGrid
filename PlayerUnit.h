#pragma once

#include "GameObject.h"
#include "GraphicsResourceManager.h"
#include "StaticMesh.h"
#include "Transform.h"

//Represents a character/unit controlled by the player
//For now basically just a wrapper for a pair of static meshes
class PlayerUnit : public GameObject
{
private:
public:
	PlayerUnit(GraphicsResourceManager* resourceManager);
	void draw(int renderPass, Transform transform);
};