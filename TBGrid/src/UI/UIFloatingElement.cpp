#include "UI/UIFloatingElement.h"

UIFloatingElement::UIFloatingElement(glm::vec2 positionOffset, glm::vec2 size, glm::vec3 worldPosition, Camera* sceneCamera)
	: UIElement(positionOffset, size),
	worldPosition({ worldPosition.x, worldPosition.y, worldPosition.z, 1.0f }),
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