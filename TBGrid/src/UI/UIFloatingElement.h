#pragma once

#include "glm/glm.hpp"
#include "Camera.h"
#include "Shader.h"
#include "UI/UIElement.h"

//class for a UI element attached to a world-space location.
//It updates it's canvas location to hover over the targeted spot.
//Does not account for the target location's depth into the scene.

class UIFloatingElement : public UIElement
{
private:
	Camera* camera;
public:
	UIFloatingElement(glm::vec2 positionOffset, glm::vec2 size, glm::vec3 worldPosition, Camera* sceneCamera);
	glm::vec2 computeEffectivePosition() const override;
	glm::vec4 worldPosition;
};