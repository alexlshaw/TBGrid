#include "RangedAttackAction.h"
#include "Scene.h"

RangedAttackAction::RangedAttackAction(TurnBoundUnit* unit, TurnBoundUnit* target, Scene* scene)
	: Action(unit),
	scene(scene)
{
	glm::vec3 startingPosition = unit->transform.getPosition() + Unit::TARGET_OFFSET;
	glm::vec3 targetLocation = target->transform.getPosition() + Unit::TARGET_OFFSET;
	glm::vec3 dir = glm::normalize(targetLocation - startingPosition);
	startingPosition += dir * 0.75f;	//uncertain how far away to spawn the projectile
	Transform transform(startingPosition, glm::identity<mat4>(), glm::vec3(0.05f, 0.05f, 0.05f));
	projectile = std::make_shared<Projectile>(transform, 2.0f, 15.0f, targetLocation, "unit_sphere", "DefaultLit");
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