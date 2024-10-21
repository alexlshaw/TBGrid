#pragma once

#include <vector>
#include "glm/glm.hpp"

//A TurnBoundUnit object has an action, but actions modify the TurnBoundUnit, so they need to know about each other
class TurnBoundUnit;

class Action
{
protected:
	TurnBoundUnit* unit;				//Action has no ownership over unit, we just need to know about it
public:
	Action(TurnBoundUnit* unit);
	~Action(){}
	virtual bool processAction(const float deltaTime) = 0;	//We require that action sub-types provide their own process implementation
};

class MovementAction : public Action
{
private:
	std::vector<glm::vec3> movementRoute;
	int movementTargetIndex = 0;		//the current location on the route the unit is aiming for
public:
	MovementAction(TurnBoundUnit* unit, std::vector<glm::vec3> route);
	~MovementAction() {}
	bool processAction(const float deltaTime) override;
};

class IdleAction : public Action
{
private:
	float idleTime;
	float elapsedIdleTime = 0.0f;
public:
	IdleAction(TurnBoundUnit* unit, float idleTime);
	~IdleAction() {}
	bool processAction(const float deltaTime) override;
};