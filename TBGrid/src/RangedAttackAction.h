#pragma once

#include "Action.h"
#include "Projectile.h"
#include <memory>

class Scene;

class RangedAttackAction : public Action
{
private:
	Scene* scene;
	std::shared_ptr<Projectile> projectile;	//the projectile spawned by the attack
public:
	RangedAttackAction(TurnBoundUnit* unit, TurnBoundUnit* target, Scene* scene);
	bool processAction(const float deltaTime) override;
};