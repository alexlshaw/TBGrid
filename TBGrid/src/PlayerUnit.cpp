#include "PlayerUnit.h"
#include "BoxCollider.h"
#include "GraphicsResourceManager.h"
#include "RiggedObject.h"
#include "Transform.h"

PlayerUnit::PlayerUnit(LevelGrid* grid)
	:TurnBoundUnit(grid),
	attackActionPointCost(3)	//arbitary value
{
	GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
	name = "PlayerUnit";
	//Probably need to place a default material here
	Mesh* cube = resourceManager.loadMesh("unit_cube");
	Material* defaultRed = resourceManager.loadMaterial("DefaultRed");
	collider = std::make_unique<BoxCollider>();
	collider->offset = glm::vec3(-0.5f, 0.0f, -0.5f);
	collider->layer = Collision::Layer_Unit;

	AnimatedModel* animModel = resourceManager.loadAnimatedModel("X Bot");
	Animation* tauntAnim = resourceManager.loadAnimation("Taunt", animModel);
	std::shared_ptr<Animator> animator = std::make_shared<Animator>(tauntAnim);
	animator->playAnimation(tauntAnim);
	std::shared_ptr<RiggedObject> animatedObject = std::make_shared<RiggedObject>(animator);
	animatedObject->name = "Player Visuals";
	addChild(animatedObject);

	//Create the selection plane
	Mesh* plane = resourceManager.loadMesh("unit_plane");
	Material* selectionMaterial = resourceManager.loadMaterial("SelectionIndicator");
	selectedIndicator = std::make_shared<StaticMesh>(plane, selectionMaterial);
	selectedIndicator->name = "Player unit selection indicator";
	selectedIndicator->transform = Transform(glm::vec3(-0.5f, 0.025f, -0.5f), glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	selectedIndicator->enabled = false;
	selectedIndicator->castsShadows = false;
	addChild(selectedIndicator);
}

void PlayerUnit::update(float deltaTime)
{
	//for now, we only need to process the action, but that will change in the future
	if (processAction(deltaTime))
	{
		action.reset();
		action = nullptr;
	}
}

void PlayerUnit::activateAbility(int abilityID)
{
	DEBUG_PRINTLN(std::format("{} activated ability: {}", name, abilityID));
}
