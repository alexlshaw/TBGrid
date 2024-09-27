#include "SphereCollider.h"

SphereCollider::SphereCollider() {}

SphereCollider::~SphereCollider() {}

bool SphereCollider::testPoint(glm::vec3 point, Transform& transform)
{
	//apply the inverse transformation to the point we want to test, then test against unit sphere
	glm::mat4 inverseTransform = transform.getInverseTransformationMatrix();
	glm::vec3 transformedPoint = glm::vec3(inverseTransform * glm::vec4(point.x, point.y, point.z, 1.0f));
	transformedPoint -= offset;
	return Collision::pointInUnitSphere(transformedPoint);

}

bool SphereCollider::testRay(glm::vec3 origin, glm::vec3 direction, Transform& transform, glm::vec3& hitLocation)
{
	glm::mat4 inverseTransform = transform.getInverseTransformationMatrix();
	glm::vec3 transformedOrigin = glm::vec3(inverseTransform * glm::vec4(origin.x, origin.y, origin.z, 1.0f));	//w=1 for position vector
	transformedOrigin -= offset;
	glm::vec3 transformedDirection = glm::vec3(inverseTransform * glm::vec4(direction.x, direction.y, direction.z, 0.0f));	//w=0 for direction vector
	glm::vec3 hitNormal;	//For the time being, our collider doesn't care about the hit normal, so we just declare, use, and immediately discard
	bool result = Collision::raySphereCollision(transformedOrigin, transformedDirection, glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, hitLocation, hitNormal);
	{
		//we've hit the thing, now we need to put our hit location back into the original coordinate space
		hitLocation += offset;	//readjust for the offset
		hitLocation = glm::vec3(transform.getMatrix() * glm::vec4(hitLocation.x, hitLocation.y, hitLocation.z, 1.0f));
	}
	return result;
}