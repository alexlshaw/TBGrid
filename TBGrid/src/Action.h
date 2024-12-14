#pragma once

#include <vector>
#include "glm/glm.hpp"

//A TurnBoundUnit object has an action, but actions modify the TurnBoundUnit, so they need to know about each other
class TurnBoundUnit;

//Although we split action behaviour into subclasses, some stuff (e.g. animations) cares about what type of action the unit is currently executing
//and having actions expose their type is more convenient than doing a cast to check type each time
namespace Actions
{
	enum ActionType
	{
		Default,	//Shouldn't actually be used since we don't create instances of the generic Action class
		Movement,
		RangedAttack,
		Idle
	};
}

class Action
{
protected:
	TurnBoundUnit* unit;				//Action has no ownership over unit, we just need to know about it
public:
	Action(TurnBoundUnit* unit, Actions::ActionType type);
	~Action(){}
	//We require that action sub-types provide their own process implementation
	//processAction should return true when the action completes
	virtual bool processAction(const float deltaTime) = 0;
	Actions::ActionType actionType;
};

class MovementAction : public Action
{
private:
	std::vector<glm::vec3> movementRoute;
	int movementTargetIndex = 0;		//the current location on the route the unit is aiming for
	void updateUnitFacing();
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