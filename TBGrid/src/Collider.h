#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "Collision.h"
#include "Transform.h"

namespace Collision
{
	enum CollisionLayer
	{
		Layer_Unknown = 1,
		Layer_LevelGeometry = 2,
		Layer_Unit = 4,
		Layer_All = 8
	};
}


//This is attached to gameobjects to enable them to be tested for collision
//This is the superclass for more specific colliders
class Collider
{
private:
public:
	Collider() {};
	~Collider() {};
	Collision::CollisionLayer layer = Collision::Layer_Unknown;
	float maximumExtent = 1.0f;	//Used for quick distance checks to rule out collisions without doing a full computation
	glm::vec3 offset = glm::vec3();	//an offset to the position of the collider within the coordinate space of the owning object (essentially a pseudo-transform for the collider itself)
	virtual bool testPoint(const glm::vec3 point, Transform& transform) const = 0;
	virtual bool testRay(const glm::vec3 origin, const glm::vec3 direction, Transform& transform, glm::vec3& hitLocation) const = 0;
	//Very basic test: returns true if objects are close enough to warrant further investigation. Should expect many false positives, but not false negatives
	bool quickTest(const Collider& other, const Transform& othersTransform, const Transform& ownersTransform) const
	{
		glm::vec3 ownerScale = ownersTransform.getScale();
		glm::vec3 otherScale = othersTransform.getScale();
		float effectiveExtent = maximumExtent * std::max({ ownerScale.x, ownerScale.y, ownerScale.z });
		float effectiveOtherExtent = other.maximumExtent * std::max({ otherScale.x, otherScale.y, otherScale.z });
		float distance = glm::length((othersTransform.getPosition() + other.offset) - (ownersTransform.getPosition() + offset));
		return distance <= effectiveExtent + effectiveOtherExtent;
	}
	//If the quicktest can't rule an object out, we have to do the Collider type - specific slow test
	virtual bool slowTest(const Collider* other, Transform& othersTransform, Transform& ownersTransform) const = 0;
	//Add other collision tests as necessary
};