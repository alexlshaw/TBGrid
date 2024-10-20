#pragma once

#include "glm\glm.hpp"
#include "GraphicsResourceManager.h"
#include "Text2d.h"
#include "UIElement.h"

#include <memory>
#include <vector>

class UICanvas
{
private:
	//Because all ui elements are at their core a single quad in screenspace (for now)
	//We can save a bunch of draw calls by having the all elements within the canvas share the same vertices
	unsigned int vao, vbo;
	Shader* basicUIShader = nullptr;
	std::vector<std::shared_ptr<UIElement>> uiElements;
public:
	UICanvas(glm::ivec2 screenSize);
	~UICanvas();
	void addElement(std::shared_ptr<UIElement> element);
	void draw();
	FontAtlas defaultFont;
	glm::mat4 orthoProjection;
};