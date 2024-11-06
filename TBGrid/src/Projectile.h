#pragma once

#include <functional>
#include <memory>
#include <string>
#include "GameObject.h"
#include "StaticMesh.h"

struct AttackInfo;	//AttackInfo knows about gameobject, so circular ref

class Projectile : public GameObject
{
private:
	float maxLife;
	float elapsedLife = 0.0f;
	float projectileSpeed;
	glm::vec3 targetLocation;
	glm::vec3 forwardDirection;	//Because we might mess with the orientation of the transform, we can't rely on transform.getForward() to be the direction in which the projectile should be travelling
public:
	Projectile(Transform& startingTransform, float projectileSpeed, float maxLife, glm::vec3 targetLocation, std::string mesh, std::string material);
	std::function<void(GameObject*)> onHit;
	void update(float deltaTime) override;
	bool hitTarget = false;
	void onCollision(GameObject* otherObject) override;
	std::unique_ptr<AttackInfo> attackInfo;
};