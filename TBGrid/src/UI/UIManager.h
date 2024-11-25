#pragma once

#include <format>
#include <memory>
#include <vector>
#include "PlayerUnit.h"
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
	void buildSelectedUnitUI();
	//variables related to action points
	std::shared_ptr<UIElement> actionPointWrapper;	//an empty parent for the AP icons so they can be acted on as a group more easily
	std::vector<std::shared_ptr<UIImageElement>> actionPointIcons;
	std::vector<std::shared_ptr<UIImageElement>> actionPointEmptyIcons;
	int availableActionPoints = 0;
	int projectedAPCost = 0;
	float APanimationTimer = 0.0f;
	//variables related to abilities
	std::shared_ptr<UIImageElement> abilitiesPanel;	//Wrapper for the ability icons
	std::vector<std::shared_ptr<UIButtonElement>> abilityButtons;
public:
	UIManager(glm::ivec2 screenSize);
	~UIManager() {}
	void update(const float deltaTime);	//Some elements have animation etc
	void showDebugInfo(bool show);
	void setDebugText(std::string_view cameraDetails, int fps);
	void setTurnInfo(bool playerTurn);
	void populateUIForSelectedUnit(PlayerUnit* unit);
	void updateActionPointUI(PlayerUnit* unit, const int projectedAPcost);
	void updateAbilityUI(PlayerUnit* unit);
	std::shared_ptr<UICanvas> mainCanvas = nullptr;
	void setGameManager(GameManager* gameManager);
};