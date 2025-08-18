#pragma once

#include "Animator.h"
#include "Billboard.h"
#include "StaticMesh.h"
#include "TurnBoundUnit.h"

//Represents a character/unit controlled by the player
class PlayerUnit : public TurnBoundUnit
{
private:
	Animator* visualsAnimator = nullptr;
	std::shared_ptr<Billboard> healthBar = nullptr;
	void createCollider();
	void createAnimator();
	void createSelectionPlane();
	void createHealthBar();
public:
	PlayerUnit(LevelGrid* grid);
	void update(float deltaTime);
	std::shared_ptr<StaticMesh> selectedIndicator = nullptr;
	
	int attackActionPointCost;

	void activateAbility(int abilityID);
	void updateHealthBarDisplay() override;
};