#pragma once

#include "glm/glm.hpp"
#include "Collision.h"
#include "Transform.h"

//This is attached to gameobjects to enable them to be tested for collision
//This is the superclass for more specific colliders
class Collider
{
private:
public:
	Collider() {};
	~Collider() {};
	virtual bool testPoint(glm::vec3 point, Transform& transform) = 0;
	virtual bool testRay(glm::vec3 origin, glm::vec3 direction, Transform& transform, glm::vec3& hitLocation) = 0;
	//Add other collision tests as necessary
};