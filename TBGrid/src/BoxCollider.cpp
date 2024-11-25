#include "BoxCollider.h"

namespace ColliderValues
{
	constexpr float BOX_MAX_EXTENT = 1.7320508f;	//sqrt(3), max diagonal of unit cube
}

BoxCollider::BoxCollider()
{
	maximumExtent = ColliderValues::BOX_MAX_EXTENT;
}

BoxCollider::~BoxCollider() {}

bool BoxCollider::testPoint(const glm::vec3 point, Transform& transform) const
{
	//apply the inverse transformation to the point we want to test, then test against unit cube (0...1)
	glm::mat4 inverseTransform = transform.getInverseTransformationMatrix();
	glm::vec3 transformedPoint = glm::vec3(inverseTransform * glm::vec4(point.x, point.y, point.z, 1.0f));
	transformedPoint -= offset;
	glm::vec3 b1(0.0f, 0.0f, 0.0f), b2(1.0f, 1.0f, 1.0f);
	return Collision::inBox(transformedPoint, b1, b2);
}

bool BoxCollider::testRay(const glm::vec3 origin, const glm::vec3 direction, Transform& transform, glm::vec3& hitLocation) const
{
	//Apply the inverse transformation to the ray
	glm::mat4 inverseTransform = transform.getInverseTransformationMatrix();
	glm::vec3 transformedOrigin = glm::vec3(inverseTransform * glm::vec4(origin.x, origin.y, origin.z, 1.0f));				//w=1 for position vector
	transformedOrigin -= offset;
	glm::vec3 transformedDirection = glm::vec3(inverseTransform * glm::vec4(direction.x, direction.y, direction.z, 0.0f));	//w=0 for direction vector
	//Compute a line to test against the box, create one that will definitely be long enough to hit
	float lineLength = glm::length(origin - transform.getPosition()) * 2.0f;	
	glm::vec3 b1(0.0f, 0.0f, 0.0f), b2(1.0f, 1.0f, 1.0f);

	bool result = Collision::checkLineBox(b1, b2, transformedOrigin, transformedOrigin + lineLength * transformedDirection, hitLocation);
	if (result)
	{
		//we've hit the thing, now we need to put our hit location back into the original coordinate space
		hitLocation += offset;	//readjust for the offset
		hitLocation = glm::vec3(transform.getMatrix() * glm::vec4(hitLocation.x, hitLocation.y, hitLocation.z, 1.0f));
	}
	return result;
}

bool BoxCollider::slowTest(const Collider* other, Transform& othersTransform, Transform& ownersTransform) const
{
	//To test if we overlap another collider, we can just check our 8 corners against their standard point-collision test
	std::vector<glm::vec3> myCorners = getCorners();
	for (int i = 0; i < 8; i++)
	{
		glm::vec3 corner = (ownersTransform.getMatrix() * glm::vec4(myCorners[i].x, myCorners[i].y, myCorners[i].z, 1.0f));
		if (other->testPoint(corner, othersTransform))
		{
			return true;
		}
	}
	//if the other collider is entirely within this collider such that all 8 corners are outside it, this will have so far registered negative
	//We can handle this by just checking the other objects origin using our point-collision check
	//There are still possible cases that this will miss (e.g. two boxes that intersect slightly along the midpoint of their edges like X), but it's good enough for now
	return testPoint(othersTransform.getPosition(), ownersTransform);
}

std::vector<glm::vec3> BoxCollider::getCorners() const
{
	return
	{
		offset + glm::vec3{0.0f, 0.0f, 0.0f},
		offset + glm::vec3{0.0f, 0.0f, 1.0f},
		offset + glm::vec3{1.0f, 0.0f, 0.0f},
		offset + glm::vec3{1.0f, 0.0f, 1.0f},
		offset + glm::vec3{0.0f, 1.0f, 0.0f},
		offset + glm::vec3{0.0f, 1.0f, 1.0f},
		offset + glm::vec3{1.0f, 1.0f, 0.0f},
		offset + glm::vec3{1.0f, 1.0f, 1.0f}
	};
}

glm::vec3 BoxCollider::getCentre() const
{
	return glm::vec3(0.5f, 0.5f, 0.5f) + offset;
}
