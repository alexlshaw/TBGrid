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
	GraphicsResourceManager* resourceManager;
	Shader* textShader = nullptr;
	std::vector<std::shared_ptr<UITextElement>> uiElements;
public:
	UICanvas(GraphicsResourceManager* resourceManager, glm::mat4 orthoProjection);
	~UICanvas();
	void addElement(std::shared_ptr<UITextElement> element);
	void draw();
	FontAtlas defaultFont;
	glm::mat4 orthoProjection;
};