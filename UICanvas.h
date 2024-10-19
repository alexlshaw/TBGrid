#pragma once

#include "glm\glm.hpp"
#include "GraphicsResourceManager.h"
#include "Text2d.h"
#include "UITextElement.h"

#include <memory>
#include <vector>

class UICanvas
{
private:
	Shader* textShader = nullptr;
	std::vector<std::shared_ptr<UITextElement>> uiElements;
public:
	UICanvas(glm::ivec2 screenSize);
	~UICanvas();
	void addElement(std::shared_ptr<UITextElement> element);
	void draw();
	FontAtlas defaultFont;
	glm::mat4 orthoProjection;
};