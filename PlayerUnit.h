#pragma once

#include "BoxCollider.h"
#include "GameObject.h"
#include "GraphicsResourceManager.h"
#include "StaticMesh.h"
#include "Transform.h"

const static float MOVEMENT_SPEED = 2.0f;	//Movement speed in tiles/s (which is also units/s as tiles are 1 unit wide)

//Represents a character/unit controlled by the player
//For now basically just a wrapper for a pair of static meshes
class PlayerUnit : public GameObject
{
private:
	glm::vec3 movementTarget = glm::vec3();
public:
	bool hasAction = false;
	void update(float deltaTime);
	void assignMovementAction(glm::vec3 targetLocation);
	bool processAction(float deltaTime);	//make progress on any action this unit is involved in. Returns true when the action completes
	std::shared_ptr<StaticMesh> selectedIndicator = nullptr;
	PlayerUnit(GraphicsResourceManager* resourceManager);
};