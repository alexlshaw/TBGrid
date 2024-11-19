#pragma once

#include "GameObject.h"
#include "Light.h"
#include "Mesh.h"
#include "glm/glm.hpp"

class LightMarker : public GameObject
{
private:
	Mesh* mesh;
	glm::vec4 lightColor;
	static glm::vec4 computeLightColor(const PointLight& light);
public:
	LightMarker(PointLight& light);
	void draw(int renderPass);
};