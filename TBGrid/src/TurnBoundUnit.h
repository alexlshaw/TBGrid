#pragma once

#include <memory>
#include "Action.h"
#include "RangedAttackAction.h"
#include "GameObject.h"
#include "LevelGrid.h"

struct AttackInfo;
class Scene;

namespace Unit
{
	const static float DEFAULT_MOVEMENT_SPEED = 2.0f;	//Movement speed in tiles/s (which is also units/s as tiles are 1 unit wide)
	const static glm::vec3 CELL_OFFSET{ 0.5f, 0.1f, 0.5f };	//In order for a player unit to appear normal, this is the offset added to the position of a level tile to get their position within it
	const static glm::vec3 TARGET_OFFSET(0.0f, 0.5f, 0.0f);	//We don't want to aim projectiles at a unit's feet
	const static float DEFAULT_MAX_HP = 100.0f;
	const static float DEFAULT_HP_BAR_WIDTH = 0.8f;
	const static float DEFAULT_HP_BAR_HEIGHT = 0.05f;
}

//Parent class for any units on the field that obey the general rules around turns and can be assigned actions
class TurnBoundUnit : public GameObject
{
protected:
	LevelGrid* levelGrid = nullptr;
	std::shared_ptr<Action> action = nullptr;
public:
	TurnBoundUnit(LevelGrid* grid);
	~TurnBoundUnit() {}
	void assignMovementAction(std::vector<glm::vec3> targetRoute);
	void assignIdleAction();
	void assignAttackAction(TurnBoundUnit* target, Scene* scene);
	bool processAction(const float deltaTime);	//make progress on any action this unit is involved in. Returns true when the action completes
	int remainingActionPoints;	//Is this unit free to select or be assigned an action?
	void resetActionPoints();	//sets action points to appropriate value as per start of turn
	bool hasAction() const;
	float maxHP = Unit::DEFAULT_MAX_HP;
	float movementSpeed = Unit::DEFAULT_MOVEMENT_SPEED;
	float currentHP = maxHP;
	virtual void receiveHit(AttackInfo* attack);
	virtual void onDeath();
	virtual void updateHealthBarDisplay();
};