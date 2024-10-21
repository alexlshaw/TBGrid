#include "UIManager.h"
#include "GameManager.h"

void UIManager::buildMainUI()
{
	GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
	mainCanvas = std::make_shared<UICanvas>(screenSize);

	Texture* panelTex = resourceManager.loadTexture("BaseTex");
	statsPanel = std::make_shared<UIImageElement>(glm::vec2(0.0f, screenSize.y - 64.0f), glm::vec2(screenSize.x, 64.0f), panelTex);
	statsPanel->enabled = false;
	mainCanvas->addElement(statsPanel);

	glm::vec3 debugTextColor(0.1f, 0.2f, 0.1f);
	cameraInfo = std::make_shared<UITextElement>("CamPosition", glm::vec2(25.0f, 44.0f), 0.4f, debugTextColor);
	statsPanel->addChild(cameraInfo);
	renderInfo = std::make_shared<UITextElement>("FPS", glm::vec2(25.0f, 10.0f), 0.4f, debugTextColor);
	statsPanel->addChild(renderInfo);

	turnInfo = std::make_shared<UITextElement>("Your turn", glm::vec2(10.0f, 10.0f), 0.5f, debugTextColor);
	mainCanvas->addElement(turnInfo);

	advanceTurnButton = std::make_shared<UIButtonElement>(glm::vec2(screenSize.x - 80.0f, 0.0f), glm::vec2(80.0f, 80.0f), panelTex, ">>");
	advanceTurnButton->onClick = [this]() {
		if (gameManager->playerTurn)
		{
			gameManager->switchTurn();
		}};
	mainCanvas->addElement(advanceTurnButton);
}

UIManager::UIManager(glm::ivec2 screenSize)
	:	screenSize(screenSize)
{
	buildMainUI();
}

void UIManager::showDebugInfo(bool show)
{
	statsPanel->enabled = show;
}

void UIManager::setDebugText(std::string_view cameraDetails, int fps)
{
	cameraInfo->text = cameraDetails;
	renderInfo->text = std::format("FPS: {}", fps);
}

void UIManager::setTurnInfo(bool playerTurn)
{
	turnInfo->text = playerTurn ? "Your Turn" : "Enemy Turn";
}

void UIManager::setGameManager(GameManager* gameManager)
{
	this->gameManager = gameManager;
}
