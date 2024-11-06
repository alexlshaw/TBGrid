#pragma once

#include <memory>
#include "Action.h"
#include "RangedAttackAction.h"
#include "GameObject.h"
#include "LevelGrid.h"

class Scene;

namespace Unit
{
	const static float MOVEMENT_SPEED = 2.0f;	//Movement speed in tiles/s (which is also units/s as tiles are 1 unit wide)
	const static glm::vec3 CELL_OFFSET{ 0.5f, 0.1f, 0.5f };	//In order for a player unit to appear normal, this is the offset added to the position of a level tile to get their position within it
	const static glm::vec3 TARGET_OFFSET(0.0f, 0.5f, 0.0f);	//We don't want to aim projectiles at a unit's feet
}

//Parent class for any units on the field that obey the general rules around turns and can be assigned actions
class TurnBoundUnit : public GameObject
{
protected:
	std::shared_ptr<Action> action = nullptr;
public:
	TurnBoundUnit() {}
	~TurnBoundUnit() {}
	void assignMovementAction(std::vector<glm::vec3> targetRoute, LevelGrid& grid);
	void assignIdleAction();
	void assignAttackAction(TurnBoundUnit* target, Scene* scene);
	bool processAction(const float deltaTime);	//make progress on any action this unit is involved in. Returns true when the action completes
	bool actionAvailable = true;	//Is this unit free to select or be assigned an action?
	bool hasAction() const;
};