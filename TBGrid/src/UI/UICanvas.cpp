#include "UICanvas.h"

UICanvas::UICanvas(glm::ivec2 screenSize)
	:	screenSize(screenSize)
{
	orthoProjection = glm::ortho(0.0f, static_cast<float>(screenSize.x), 0.0f, static_cast<float>(screenSize.y));
	basicUIShader = GraphicsResourceManager::getInstance().loadShader("ui/basicUI");
	
	//Generate shared UI Quad
	float xpos = 0.0;
	float ypos = 0.0;
	float l = 1.0f;
	std::vector<UIVertex> vertices = {
		//Default vertices we use when drawing most elements
		UIVertex{glm::vec2{xpos, ypos}, glm::vec2{0.0f, 0.0f}},
		UIVertex{glm::vec2{xpos + l, ypos + l}, glm::vec2{1.0f, 1.0f}},
		UIVertex{glm::vec2{xpos, ypos + l}, glm::vec2{0.0f, 1.0f}},
		UIVertex{glm::vec2{xpos, ypos}, glm::vec2{0.0f, 0.0f}},
		UIVertex{glm::vec2{xpos + l, ypos}, glm::vec2{1.0f, 0.0f}},
		UIVertex{glm::vec2{xpos + l, ypos + l}, glm::vec2{1.0f, 1.0f}},
		//vertices with inverted texture coords we use when drawing text elements
		UIVertex{glm::vec2(xpos,ypos + l), glm::vec2(0.0f, 0.0f)},
		UIVertex{glm::vec2(xpos,ypos), glm::vec2(0.0f,1.0f)},
		UIVertex{glm::vec2(xpos + l,ypos), glm::vec2(1.0f, 1.0f)},
		UIVertex{glm::vec2(xpos,ypos + l), glm::vec2(0.0f, 0.0f)},
		UIVertex{glm::vec2(xpos + l,ypos), glm::vec2(1.0f,1.0f)},
		UIVertex{glm::vec2(xpos + l,ypos + l), glm::vec2(1.0f, 0.0f)}
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertex) * 12, &vertices[0], GL_DYNAMIC_DRAW);
	setUIVertexAttribs();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void UICanvas::addElement(std::shared_ptr<UIElement> element)
{
	uiElements.push_back(element);
}

void UICanvas::draw()
{
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);
	glEnable(GL_BLEND);
	for (auto& element : uiElements)
	{
		if (element->enabled)
		{
			//Note that certain UI elements may provide their own shader here
			element->drawAll(basicUIShader, orthoProjection);
		}
	}
	glDisable(GL_BLEND);
	glBindVertexArray(0);
}

void UICanvas::update(const float deltaTime)
{
	for (auto& element : uiElements)
	{
		element->updateAll(deltaTime);
	}
}

bool UICanvas::onClick(glm::vec2 clickLocation)
{
	//convert the click location into the canvas coordinate space (inverted y)
	glm::vec2 canvasLocation(clickLocation.x, screenSize.y - clickLocation.y);
	//Iterate through all of our objects, and see if they are able to consume the click
	for (auto& element : uiElements)
	{
		if (element->enabled && element->consumeClick(canvasLocation))
		{
			return true;
		}
	}
	return false;
}