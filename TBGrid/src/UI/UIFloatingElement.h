#pragma once

#include "glm/glm.hpp"
#include "Camera.h"
#include "Shader.h"
#include "UI/UIElement.h"

//class for a UI element attached to a world-space location.
//It updates it's canvas location to hover over the targeted spot.
//A little different to traditional UI elements - the size property should track the size in pixels of all content
//Then we also track a target world size, so that we can scale the content accordingly
//Note that this just considers the ratio of world/pixel size in the x axis for computing scale
class UIFloatingElement : public UIElement
{
private:
	Camera* camera;
public:
	UIFloatingElement(glm::vec2 positionOffset, glm::vec2 size, glm::vec2 targetWorldSize, glm::vec3 worldPosition, Camera* sceneCamera);
	glm::vec2 computeEffectivePosition() const override;
	glm::vec2 computeEffectiveScale() const override;
	glm::vec3 worldPosition;	//What is the world location to which this element is anchored?
	glm::vec2 targetWorldSize;	//How large should the element appear in world units?
};