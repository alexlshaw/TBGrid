#pragma once

#include "Collider.h"

//By default, our box collider is a w/d/h 1 cube centered on the origin of its associated transform
class BoxCollider : public Collider
{
private:
public:
	BoxCollider();
	~BoxCollider();
	bool testPoint(glm::vec3 point, Transform& transform);
	bool testRay(glm::vec3 origin, glm::vec3 direction, Transform& transform, glm::vec3& hitLocation);
};