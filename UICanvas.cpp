#include "UICanvas.h"

UICanvas::UICanvas(glm::ivec2 screenSize)
{
	orthoProjection = glm::ortho(0.0f, static_cast<float>(screenSize.x), 0.0f, static_cast<float>(screenSize.y));
	textShader = GraphicsResourceManager::getInstance().loadShader("ui/text2d");
	defaultFont = createFontTextureAtlas("Data/Fonts/Poly-Regular.otf");
}

UICanvas::~UICanvas() 
{
	if (defaultFont.textureID)
	{
		glDeleteTextures(1, &defaultFont.textureID);
	}
}

void UICanvas::addElement(std::shared_ptr<UITextElement> element)
{
	uiElements.push_back(element);
}

void UICanvas::draw()
{
	glEnable(GL_BLEND);
	for (auto& element : uiElements)
	{
		if (element->enabled)
		{
			element->draw(textShader, orthoProjection);
		}
	}
	glDisable(GL_BLEND);
}
