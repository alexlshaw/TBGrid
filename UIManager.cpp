#include "UIManager.h"

void UIManager::buildMainUI()
{
	mainCanvas = std::make_shared<UICanvas>(screenSize);
	glm::vec3 debugTextColor(0.1f, 0.3f, 0.1f);
	cameraInfo = std::make_shared<UITextElement>("CamPosition", glm::vec2(15.0f, screenSize.y - 20.0f), 0.4f, debugTextColor, &(mainCanvas->defaultFont));
	cameraInfo->enabled = false;
	mainCanvas->addElement(cameraInfo);
	renderInfo = std::make_shared<UITextElement>("FPS", glm::vec2(15.0f, screenSize.y - 44.0f), 0.4f, debugTextColor, &(mainCanvas->defaultFont));
	renderInfo->enabled = false;
	mainCanvas->addElement(renderInfo);
}

UIManager::UIManager(glm::ivec2 screenSize)
	:	screenSize(screenSize)
{
	buildMainUI();
}

UIManager::~UIManager() {}

void UIManager::showDebugInfo(bool show)
{
	cameraInfo->enabled = show;
	renderInfo->enabled = show;
}

void UIManager::setDebugText(std::string_view cameraDetails, int fps)
{
	cameraInfo->text = cameraDetails;
	renderInfo->text = std::format("FPS: {}", fps);
}
