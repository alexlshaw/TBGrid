#pragma once

#include <vector>
#include "Collider.h"

//By default, our sphere collider is a radius 1 sphere centered on the origin of its
//associated transform
class SphereCollider : public Collider
{
private:
public:
	SphereCollider();
	~SphereCollider();
	bool testPoint(const glm::vec3 point, Transform& transform) const override;
	bool testRay(glm::vec3 origin, glm::vec3 direction, Transform& transform, glm::vec3& hitLocation) const override;
	bool slowTest(const Collider* other, Transform& othersTransform, Transform& ownersTransform) const;
};