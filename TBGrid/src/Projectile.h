#pragma once

#include <functional>
#include <memory>
#include <string>
#include "GameObject.h"
#include "StaticMesh.h"

class Projectile : public GameObject
{
private:
	float projectileSpeed;
	glm::vec3 targetLocation;
	glm::vec3 forwardDirection;	//Because we might mess with the orientation of the transform, we can't rely on transform.getForward() to be the direction in which the projectile should be travelling
public:
	Projectile(Transform& startingTransform, float projectileSpeed, glm::vec3 targetLocation, std::string mesh, std::string material);
	std::function<void(GameObject*)> onHit;
	void update(float deltaTime) override;
	bool hitTarget = false;
	void onCollision(GameObject* otherObject) override;
};