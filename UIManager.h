#pragma once

#include <format>
#include <memory>

#include "GraphicsResourceManager.h"
#include "UICanvas.h"
#include "UIButtonElement.h"
#include "UIImageElement.h"
#include "UITextElement.h"

class GameManager;	//The game manager and UI manager both need to instruct the other, so circular dependency here

//This class is responsible for the ongoing state of the UI
//It tracks specific elements and has functions for interacting with specific parts of the UI
//In other words, it represents the game's view of the UI, rather than that of the underlying engine
class UIManager
{
private:
	GameManager* gameManager;
	glm::ivec2 screenSize;
	std::shared_ptr<UIImageElement> statsPanel;
	std::shared_ptr<UITextElement> cameraInfo;
	std::shared_ptr<UITextElement> renderInfo;
	std::shared_ptr<UITextElement> turnInfo;
	std::shared_ptr<UIButtonElement> advanceTurnButton;
	void buildMainUI();
public:
	UIManager(glm::ivec2 screenSize);
	~UIManager() {}
	void showDebugInfo(bool show);
	void setDebugText(std::string_view cameraDetails, int fps);
	void setTurnInfo(bool playerTurn);
	std::shared_ptr<UICanvas> mainCanvas = nullptr;
	void setGameManager(GameManager* gameManager);
};