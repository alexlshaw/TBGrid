#include "RangedAttackAction.h"
#include "Scene.h"

RangedAttackAction::RangedAttackAction(TurnBoundUnit* unit, TurnBoundUnit* target, Scene* scene)
	: Action(unit),
	scene(scene)
{
	glm::vec3 startingPosition = unit->transform.getPosition();
	glm::vec3 dir = glm::normalize(target->transform.getPosition() - startingPosition);
	startingPosition += dir * 1.5f;	//uncertain how far away to spawn the projectile
	Transform transform(startingPosition, glm::identity<mat4>(), glm::vec3(0.5f, 0.5f, 0.5f));
	projectile = std::make_shared<Projectile>(transform, 1.0f, target->transform.getPosition(), "unit_cube", "DefaultLit");
	scene->addObject(projectile);
}

bool RangedAttackAction::processAction(const float deltaTime)
{
	//we just wait for the projectile to finish its flight
	if (projectile->hitTarget)
	{
		scene->deleteObject(projectile);
		return true;
	}
	return false;
}