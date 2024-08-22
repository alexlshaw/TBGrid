#include "BoxCollider.h"

BoxCollider::BoxCollider() {}

BoxCollider::~BoxCollider() {}

bool BoxCollider::testPoint(glm::vec3 point, Transform& transform)
{
	//apply the inverse transformation to the point we want to test, then test against unit sphere
	glm::mat4 inverseTransform = transform.getInverseTransformationMatrix();
	glm::vec3 transformedPoint = glm::vec3(inverseTransform * glm::vec4(point.x, point.y, point.z, 1.0f));
	glm::vec3 b1(0.0f, 0.0f, 0.0f), b2(1.0f, 1.0f, 1.0f);
	return Collision::inBox(transformedPoint, b1, b2, X_AXIS) &&
		Collision::inBox(transformedPoint, b1, b2, Y_AXIS) &&
		Collision::inBox(transformedPoint, b1, b2, Y_AXIS);
}

bool BoxCollider::testRay(glm::vec3 origin, glm::vec3 direction, Transform& transform, glm::vec3& hitLocation)
{
	glm::mat4 inverseTransform = transform.getInverseTransformationMatrix();
	glm::vec3 transformedOrigin = glm::vec3(inverseTransform * glm::vec4(origin.x, origin.y, origin.z, 1.0f));				//w=1 for position vector
	glm::vec3 transformedDirection = glm::vec3(inverseTransform * glm::vec4(direction.x, direction.y, direction.z, 0.0f));	//w=0 for direction vector
	
	float lineLength = glm::length(origin - transform.getPosition()) * 2.0f;	//we're testing a line against the box, so we create one that will definitely be long enough to hit
	glm::vec3 b1(0.0f, 0.0f, 0.0f), b2(1.0f, 1.0f, 1.0f);

	bool result = Collision::checkLineBox(b1, b2, transformedOrigin, transformedOrigin + lineLength * transformedDirection, hitLocation);
	if (result)
	{
		//we've hit the thing, now we need to put our hit location back into the original coordinate space
		hitLocation = glm::vec3(transform.getMatrix() * glm::vec4(hitLocation.x, hitLocation.y, hitLocation.z, 1.0f));
	}
	return result;
}