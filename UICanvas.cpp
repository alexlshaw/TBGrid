#include "UICanvas.h"

UICanvas::UICanvas(glm::ivec2 screenSize)
{
	orthoProjection = glm::ortho(0.0f, static_cast<float>(screenSize.x), 0.0f, static_cast<float>(screenSize.y));
	basicUIShader = GraphicsResourceManager::getInstance().loadShader("ui/basicUI");
	defaultFont = createFontTextureAtlas("Data/Fonts/Poly-Regular.otf");

	//Generate shared UI Quad
	float xpos = 0.0;
	float ypos = 0.0;
	float l = 1.0f;
	std::vector<UIVertex> vertices = {
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertex) * 6, &vertices[0], GL_DYNAMIC_DRAW);
	setUIVertexAttribs();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

UICanvas::~UICanvas() 
{
	if (defaultFont.textureID)
	{
		glDeleteTextures(1, &defaultFont.textureID);
	}
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
