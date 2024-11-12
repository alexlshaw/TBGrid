#include "UIManager.h"
#include "GameManager.h"

UIManager::UIManager(glm::ivec2 screenSize)
	: screenSize(screenSize),
	animationTimer(0.0f)
{
	buildMainUI();
	buildSelectedUnitUI();
}

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

void UIManager::buildSelectedUnitUI()
{
	GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
	Texture* APIcon = resourceManager.loadTexture("UI/AP_Icon");
	Texture* APIconEmpty = resourceManager.loadTexture("UI/AP_Icon_Empty");
	float width = 6.0f * 36.0f;
	actionPointWrapper = std::make_shared<UIElement>(glm::vec2(0.5f * (screenSize.x - width), screenSize.y - 34.0f), glm::vec2(width, 32.0f));
	mainCanvas->addElement(actionPointWrapper);
	for (int i = 0; i < 6; i++) //Max AP is 6
	{
		float x = static_cast<float>(i) * 36.0f;
		std::shared_ptr<UIImageElement> icon_empty = std::make_shared<UIImageElement>(glm::vec2(x, 0.0f), glm::vec2(32.0f, 32.0f), APIconEmpty);
		actionPointEmptyIcons.push_back(icon_empty);
		actionPointWrapper->addChild(icon_empty);
		std::shared_ptr<UIImageElement> icon = std::make_shared<UIImageElement>(glm::vec2(x, 0.0f), glm::vec2(32.0f, 32.0f), APIcon);
		actionPointIcons.push_back(icon);
		actionPointWrapper->addChild(icon);
		
	}
	populateUIForSelectedUnit(nullptr);	//we don't start with anything selected, so hide everything
}

void UIManager::update(const float deltaTime)
{
	animationTimer += deltaTime;
	mainCanvas->update(deltaTime);
	if (projectedAPCost > 0)
	{
		float factor = sinf(animationTimer * 4.0f);
		for (int i = availableActionPoints - projectedAPCost; i < availableActionPoints; i++)
		{
			actionPointIcons[i]->enabled = factor >= 0.0f;
			actionPointEmptyIcons[i]->enabled = factor < 0.0f;
		}
	}
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

void UIManager::populateUIForSelectedUnit(PlayerUnit* unit)
{
	updateActionPointUI(unit, 0);
}

void UIManager::updateActionPointUI(PlayerUnit* unit, const int projectedAPcost)
{
	if (unit)
	{
		//unit selected, show and update relevant ui elements
		availableActionPoints = unit->remainingActionPoints;
		actionPointWrapper->enabled = true;
		for (size_t i = 0; i < actionPointIcons.size(); i++)
		{
			bool hasThisPoint = i < availableActionPoints;
			actionPointIcons[i]->enabled = hasThisPoint;
			actionPointEmptyIcons[i]->enabled = !hasThisPoint;
		}
	}
	else
	{
		availableActionPoints = 0;
		//no unit selected, hide relevant ui elements
		actionPointWrapper->enabled = false;
	}
	projectedAPCost = projectedAPcost;
}

void UIManager::setGameManager(GameManager* gameManager)
{
	this->gameManager = gameManager;
}
