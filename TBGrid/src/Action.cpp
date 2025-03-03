#include "Action.h"
#include "TurnBoundUnit.h"

using namespace Actions;

Action::Action(TurnBoundUnit* unit, ActionType type)
	: unit(unit),
	actionType(type)
{}

#pragma region MovementAction

void MovementAction::updateUnitFacing()
{
	//Rotate the unit to face the next target along its route
	glm::vec3 routePosition = movementRoute[movementTargetIndex];
	glm::vec3 newForward = routePosition - unit->transform.getPosition();
	newForward.y = 0.0f;
	unit->transform.setForward(newForward);
}

MovementAction::MovementAction(TurnBoundUnit* unit, std::vector<glm::vec3> route)
	: Action(unit, Movement),
	movementRoute(route)
{
	updateUnitFacing();
}

bool MovementAction::processAction(const float deltaTime)
{
	//At the moment, we handle only movement actions
	if (movementRoute[movementTargetIndex] != unit->transform.getPosition())
	{
		float totalDistance = glm::length(movementRoute[movementTargetIndex] - unit->transform.getPosition());
		float distanceThisFrame = deltaTime * unit->movementSpeed;
		if (totalDistance <= distanceThisFrame)	//TODO: at the moment movement can only complete one segment per frame at most. Unlikely to be relevant in practice.
		{
			//after we hit our current target we will have some movement left
			movementTargetIndex++;
			if (movementTargetIndex < movementRoute.size())
			{
				//we still have another stop on our route, make progress towards it
				distanceThisFrame -= totalDistance;	//update dTF to be the remaining movement after reaching the target
				totalDistance = glm::length(movementRoute[movementTargetIndex - 1] - movementRoute[movementTargetIndex]);
				float t = distanceThisFrame / totalDistance;
				glm::vec3 newPosition = glm::mix(unit->transform.getPosition(), movementRoute[movementTargetIndex], t);
				unit->transform.setPosition(newPosition);
				updateUnitFacing();	//Set the unit's direction based on the new target
			}
			else
			{
				//we reach our final location this frame
				unit->transform.setPosition(movementRoute.back());
				return true;
			}
		}
		else
		{
			//we don't have enough movement this frame to reach our current target on the route, so we just make some progress
			float t = distanceThisFrame / totalDistance;
			glm::vec3 newPosition = glm::mix(unit->transform.getPosition(), movementRoute[movementTargetIndex], t);
			unit->transform.setPosition(newPosition);
		}
	}
	return false;
}

#pragma endregion

#pragma region IdleAction

IdleAction::IdleAction(TurnBoundUnit* unit, float idleTime)
	: Action(unit, Idle),
	idleTime(idleTime)
{}

bool IdleAction::processAction(const float deltaTime)
{
	elapsedIdleTime += deltaTime;
	return elapsedIdleTime >= idleTime;
}

#pragma endregion