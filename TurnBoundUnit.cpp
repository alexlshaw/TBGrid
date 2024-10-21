#include "TurnBoundUnit.h"

void TurnBoundUnit::assignMovementAction(std::vector<glm::vec3> targetRoute)
{
	if (targetRoute.size() > 0)
	{
		hasAction = true;
		movementTargetIndex = 0;
		movementRoute = targetRoute;
	}
}

bool TurnBoundUnit::processAction(const float deltaTime)
{
	if (hasAction)
	{
		if (movementRoute[movementTargetIndex] != transform.getPosition())
		{
			float totalDistance = glm::length(movementRoute[movementTargetIndex] - transform.getPosition());
			float distanceThisFrame = deltaTime * Unit::MOVEMENT_SPEED;
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
					glm::vec3 newPosition = glm::mix(transform.getPosition(), movementRoute[movementTargetIndex], t);
					transform.setPosition(newPosition);
				}
				else
				{
					//we reach our final location this frame
					transform.setPosition(movementRoute.back());
					hasAction = false;
					return true;
				}
			}
			else
			{
				//we don't have enough movement this frame to reach our current target on the route, so we just make some progress
				float t = distanceThisFrame / totalDistance;
				glm::vec3 newPosition = glm::mix(transform.getPosition(), movementRoute[movementTargetIndex], t);
				transform.setPosition(newPosition);
			}
		}
	}
	return false;
}