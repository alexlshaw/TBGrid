#include "SphereCollider.h"
#include "BoxCollider.h"

SphereCollider::SphereCollider() {}

SphereCollider::~SphereCollider() {}

bool SphereCollider::testPoint(const glm::vec3 point, Transform& transform) const
{
	//apply the inverse transformation to the point we want to test, then test against unit sphere
	glm::mat4 inverseTransform = transform.getInverseTransformationMatrix();
	glm::vec3 transformedPoint = glm::vec3(inverseTransform * glm::vec4(point.x, point.y, point.z, 1.0f));
	transformedPoint -= offset;
	return Collision::pointInUnitSphere(transformedPoint);

}

bool SphereCollider::testRay(const glm::vec3 origin, const glm::vec3 direction, Transform& transform, glm::vec3& hitLocation) const
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

bool SphereCollider::slowTest(const Collider* other, Transform& othersTransform, Transform& ownersTransform) const
{
	//Sphere collider checks are dependent on the other collider's type
	const BoxCollider* box = dynamic_cast<const BoxCollider*>(other);
	if (box)
	{
		std::vector<glm::vec3> corners = box->getCorners();
		for (int i = 0; i < corners.size(); i++)
		{
			//compute the other colliders corner in world space
			glm::vec3 corner = (othersTransform.getMatrix() * glm::vec4(corners[i].x, corners[i].y, corners[i].z, 1.0f));
			if (testPoint(corner, ownersTransform))
			{
				return true;
			}
		}
		return false;
	}
	const SphereCollider* sphere = dynamic_cast<const SphereCollider*>(other);
	if (sphere)
	{
		//The intersection point on a ray test from the centre of this sphere to the centre of the other sphere gives us the closest point on the other sphere to this one
		//Which we can then test whether it falls inside this sphere
		glm::vec3 closestPoint{};
		glm::vec3 origin = ownersTransform.getPosition() + offset;
		glm::vec3 direction = othersTransform.getPosition() - (ownersTransform.getPosition() + offset);
		sphere->testRay(origin, direction, othersTransform, closestPoint);
		return testPoint(closestPoint, ownersTransform);
	}
	DEBUG_PRINTLN("SphereCollider attempting collision with unknown other collider type");
	return false;
}