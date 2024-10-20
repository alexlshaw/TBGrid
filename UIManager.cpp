#include "UIManager.h"

void UIManager::buildMainUI()
{
	mainCanvas = std::make_shared<UICanvas>(screenSize);

	Texture* panelTex = GraphicsResourceManager::getInstance().loadTexture("BaseTex");
	statsPanel = std::make_shared<UIImageElement>(glm::vec2(0.0f, screenSize.y - 64.0f), glm::vec2(screenSize.x, 64.0f), panelTex);
	statsPanel->enabled = false;
	mainCanvas->addElement(statsPanel);

	glm::vec3 debugTextColor(0.1f, 0.2f, 0.1f);
	cameraInfo = std::make_shared<UITextElement>("CamPosition", glm::vec2(25.0f, 44.0f), 0.4f, debugTextColor, &(mainCanvas->defaultFont));
	statsPanel->addChild(cameraInfo);
	renderInfo = std::make_shared<UITextElement>("FPS", glm::vec2(25.0f, 10.0f), 0.4f, debugTextColor, &(mainCanvas->defaultFont));
	statsPanel->addChild(renderInfo);
}

UIManager::UIManager(glm::ivec2 screenSize)
	:	screenSize(screenSize)
{
	buildMainUI();
}

UIManager::~UIManager() {}

void UIManager::showDebugInfo(bool show)
{
	statsPanel->enabled = show;
}

void UIManager::setDebugText(std::string_view cameraDetails, int fps)
{
	cameraInfo->text = cameraDetails;
	renderInfo->text = std::format("FPS: {}", fps);
}
