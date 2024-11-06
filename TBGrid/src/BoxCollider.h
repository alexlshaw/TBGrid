#pragma once

#include <vector>
#include "Collider.h"
#include "DebuggingTools.h"

//By default, our box collider is a w/d/h 1 cube centered on the origin of its associated transform
class BoxCollider : public Collider
{
private:
public:
	BoxCollider();
	~BoxCollider();
	bool testPoint(const glm::vec3 point, Transform& transform) const override;
	bool testRay(const glm::vec3 origin, const glm::vec3 direction,  Transform& transform, glm::vec3& hitLocation) const override;
	bool slowTest(const Collider* other, Transform& othersTransform, Transform& ownersTransform) const override;
	std::vector<glm::vec3> getCorners() const;
	glm::vec3 getCentre() const;
};