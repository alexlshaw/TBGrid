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
	glm::vec2 effectivePosition = camera->worldToScreen(worldPosition);
	//step 2: take into account offset and parent position
	effectivePosition += position;
	if (getParent())
	{
		effectivePosition -= getParent()->computeEffectivePosition();
	}
	return effectivePosition;
}