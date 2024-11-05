#include "TurnBoundUnit.h"
#include "Scene.h"

void TurnBoundUnit::assignMovementAction(std::vector<glm::vec3> targetRoute, LevelGrid& grid)
{
	if (targetRoute.size() > 0)
	{
		action = std::make_shared<MovementAction>(this, targetRoute);
		//update the state of the levelgrid
		int start = grid.getCellIndexFromSpatialCoords(transform.getPosition());
		int end = grid.getCellIndexFromSpatialCoords(targetRoute.back());
		grid.setOccupiedState(start, false);
		grid.setOccupiedState(end, true);
	}
	else
	{
		//Something weird has happened and it tried to give the unit a 0-length path, just idle instead
		assignIdleAction();
	}
}

void TurnBoundUnit::assignIdleAction()
{
	action = std::make_shared<IdleAction>(this, 3.0f);
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

bool TurnBoundUnit::hasAction() const
{
	return action != nullptr;
}
