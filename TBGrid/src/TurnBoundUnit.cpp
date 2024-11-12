#include "TurnBoundUnit.h"
#include "AttackInfo.h"
#include "Scene.h"

TurnBoundUnit::TurnBoundUnit(LevelGrid* grid)
	:levelGrid(grid),
	remainingActionPoints(0)
{}

void TurnBoundUnit::assignMovementAction(std::vector<glm::vec3> targetRoute)
{
	if (targetRoute.size() > 0)
	{
		action = std::make_shared<MovementAction>(this, targetRoute);
		//update the state of the levelgrid
		int start = levelGrid->getCellIndexFromSpatialCoords(transform.getPosition());
		int end = levelGrid->getCellIndexFromSpatialCoords(targetRoute.back());
		levelGrid->setOccupiedState(start, false);
		levelGrid->setOccupiedState(end, true);
	}
	else
	{
		//Something weird has happened and it tried to give the unit a 0-length path, just idle instead
		assignIdleAction();
	}
}

void TurnBoundUnit::assignIdleAction()
{
	action = std::make_shared<IdleAction>(this, 2.0f);
}

void TurnBoundUnit::assignAttackAction(TurnBoundUnit* target, Scene* scene)
{
	action = std::make_shared<RangedAttackAction>(this, target, scene);
}

bool TurnBoundUnit::processAction(const float deltaTime)
{
	if (action)
	{
		return action->processAction(deltaTime);
	}
	return false;
}

void TurnBoundUnit::resetActionPoints()
{
	remainingActionPoints = 6;	//arbitrary temp value
}

bool TurnBoundUnit::hasAction() const
{
	return action != nullptr;
}

void TurnBoundUnit::receiveHit(AttackInfo* attack)
{
	currentHP -= attack->damage;
	if (currentHP <= 0.0f)
	{
		currentHP = 0.0f;
		onDeath();
		DEBUG_PRINTLN(name + " has recieved fatal damage");
	}
}

void TurnBoundUnit::onDeath()
{
	levelGrid->setOccupiedState(levelGrid->getCellIndexFromSpatialCoords(transform.getPosition()), false);
	enabled = false;
}
