#include "UI/UIFloatingElement.h"

UIFloatingElement::UIFloatingElement(glm::vec2 positionOffset, glm::vec2 size, glm::vec2 targetWorldSize, glm::vec3 worldPosition, Camera* sceneCamera)
	: UIElement(positionOffset, size),
	worldPosition(worldPosition),
	targetWorldSize(targetWorldSize),
	camera(sceneCamera)
{
}

glm::vec2 UIFloatingElement::computeEffectivePosition() const
{
	//compute a new position based on the camera
	//step 1: screen position from camera
	bool inFrustrum;
	glm::vec2 effectivePosition = camera->worldToScreen(worldPosition, inFrustrum);
	//step 2: take into account offset and parent position. For now, we ignore frustrum, but might choose to track it and test during the draw call
	effectivePosition += position;
	if (getParent())
	{
		effectivePosition -= getParent()->computeEffectivePosition();
	}
	return effectivePosition;
}

glm::vec2 UIFloatingElement::computeEffectiveScale() const
{
	//1: compute camera depth (z)
	float z = -((camera->getViewMatrix() * glm::vec4(worldPosition, 1.0f)).z);
	//2: compute reference scale (zref) from target/pixel size ratio and projection matrix
	float zRef = (camera->screenSize.x * 0.5f) * camera->getProjectionMatrix()[0][0] * (targetWorldSize.x / size.x);
	//3: compute scale
	float zScale = zRef / z;
	glm::vec2 parentScale = getParent() ? getParent()->computeEffectiveScale() : glm::vec2(1.0f, 1.0f);
	//4: On the off chance we have set a scale for this element as well, mix it in before computing our final scale
	return glm::vec2(parentScale.x * scale.x * zScale, parentScale.y * scale.y * zScale);
}