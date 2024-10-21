#pragma once

#include <vector>

#include "GameObject.h"
#include "GraphicsResourceManager.h"

class LineRenderer : public GameObject
{
private:
	Mesh lineMesh;
	void regenerateMesh();
	std::vector<glm::vec3> segments;
	glm::vec4 colour;
public:
	LineRenderer();
	LineRenderer(glm::vec4 colour);
	~LineRenderer() {}
	void draw(int renderPass);
	void setSegments(std::vector<glm::vec3> segments);
	void setColour(glm::vec4 colour);
	void generateSegmentsFromPoints(std::vector<glm::vec3> points); //Creates a single line that extends from point to point
};