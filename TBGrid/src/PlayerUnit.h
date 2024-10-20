#pragma once

#include "BoxCollider.h"
#include "GraphicsResourceManager.h"
#include "StaticMesh.h"
#include "Transform.h"
#include "TurnBoundUnit.h"

//Represents a character/unit controlled by the player
class PlayerUnit : public TurnBoundUnit
{
private:
public:
	void update(float deltaTime);
	std::shared_ptr<StaticMesh> selectedIndicator = nullptr;
	PlayerUnit();
};