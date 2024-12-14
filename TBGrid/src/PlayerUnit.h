#pragma once

#include "Animator.h"
#include "StaticMesh.h"
#include "TurnBoundUnit.h"

//Represents a character/unit controlled by the player
class PlayerUnit : public TurnBoundUnit
{
private:
	Animator* visualsAnimator = nullptr;
public:
	PlayerUnit(LevelGrid* grid);
	void update(float deltaTime);
	std::shared_ptr<StaticMesh> selectedIndicator = nullptr;
	int attackActionPointCost;

	void activateAbility(int abilityID);
};