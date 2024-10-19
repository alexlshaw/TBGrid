#include "UICanvas.h"

UICanvas::UICanvas(GraphicsResourceManager* resourceManager, glm::mat4 orthoProjection)
	:	resourceManager(resourceManager),
		orthoProjection(orthoProjection)
{
	textShader = resourceManager->loadShader("ui/text2d");
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
