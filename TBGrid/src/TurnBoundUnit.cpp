#include "TurnBoundUnit.h"

void TurnBoundUnit::assignMovementAction(std::vector<glm::vec3> targetRoute)
{
	if (targetRoute.size() > 0)
	{
		action = std::make_shared<MovementAction>(this, targetRoute);
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
