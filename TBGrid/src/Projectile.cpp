#include "Projectile.h"
#include "SphereCollider.h"

Projectile::Projectile(Transform& startingTransform, float projectileSpeed, float maxLife, glm::vec3 targetLocation, std::string mesh, std::string material)
	: projectileSpeed(projectileSpeed),
	maxLife(maxLife),
	targetLocation(targetLocation)
{
	name = "Attack Projectile";
	collider = std::make_unique<SphereCollider>();
	dynamic = true;
	transform = startingTransform;
	forwardDirection = glm::normalize(targetLocation - startingTransform.getPosition());
	std::shared_ptr<StaticMesh> visuals = std::make_shared<StaticMesh>(mesh, material);
	visuals->name = "Projectile Visuals";
	addChild(visuals);
}

void Projectile::update(float deltaTime)
{
	//Move the projectile toward its destination, and see if we hit anything
	glm::vec3 movement = forwardDirection * projectileSpeed * deltaTime;
	transform.setPosition(transform.getPosition() + movement);
	elapsedLife += deltaTime;
	if (elapsedLife >= maxLife)
	{
		hitTarget = true;
	}
}

void Projectile::onCollision(GameObject* otherObject)
{
	//What did we just hit?
	Collision::CollisionLayer otherLayer = otherObject->collider->layer;
	if (otherLayer & Collision::Layer_Unit)	//bitwise test layer mask
	{
		//it's a unit, we're going to (eventually) damage it or something
		hitTarget = true;
	}
	else
	{
		//At the moment there's nothing that we could hit that we wouldn't still consider the collision a "hit"
		hitTarget = true;
	}
}
