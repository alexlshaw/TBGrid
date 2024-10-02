#include "PlayerUnit.h"

PlayerUnit::PlayerUnit(GraphicsResourceManager* resourceManager)
{
	name = "PlayerUnit";
	//Probably need to place a default material here
	Mesh* cube = resourceManager->loadMesh("unit_cube");
	Material* defaultRed = resourceManager->loadMaterial("DefaultRed");
	collider = std::make_unique<BoxCollider>();
	collider->offset = glm::vec3(-0.5f, 0.0f, -0.5f);

	std::shared_ptr<StaticMesh> pillar = std::make_shared<StaticMesh>(cube, defaultRed);
	pillar->name = "Player Unit pillar";
	pillar->transform = Transform(glm::vec3(-0.25f, 0.0f, -0.25f), glm::identity<glm::mat4>(), glm::vec3(0.5f, 0.8f, 0.5f));
	addChild(pillar);

	std::shared_ptr<StaticMesh> flatBase = std::make_shared<StaticMesh>(cube, defaultRed);
	flatBase->name = "Player Unit base";
	flatBase->transform = Transform(glm::vec3(-0.4f, 0.0f, -0.4f), glm::identity<glm::mat4>(), glm::vec3(0.8f, 0.1f, 0.8f));
	addChild(flatBase);
	

	//Create the selection plane
	Mesh* plane = resourceManager->loadMesh("unit_plane");
	Material* selectionMaterial = resourceManager->loadMaterial("SelectionIndicator");
	selectedIndicator = std::make_shared<StaticMesh>(plane, selectionMaterial);
	selectedIndicator->name = "Player unit selection indicator";
	selectedIndicator->transform = Transform(glm::vec3(-0.5f, 0.025f, -0.5f), glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	selectedIndicator->enabled = false;
	addChild(selectedIndicator);
}

void PlayerUnit::assignMovementAction(glm::vec3 targetLocation)
{
	hasAction = true;
	movementTarget = targetLocation;
}

void PlayerUnit::update(float deltaTime)
{
	//for now, we only need to process the action, but that will change in the future
	processAction(deltaTime);
}

bool PlayerUnit::processAction(float deltaTime)
{
	if (hasAction)
	{
		if (movementTarget != transform.getPosition())
		{
			float totalDistance = glm::length(movementTarget - transform.getPosition());
			float distanceThisFrame = deltaTime * MOVEMENT_SPEED;
			float t = glm::min(1.0f, distanceThisFrame / totalDistance);
			glm::vec3 newPosition = glm::mix(transform.getPosition(), movementTarget, t);
			transform.setPosition(newPosition);
			if (newPosition == movementTarget)
			{
				hasAction = false;
				return true;
			}
		}
	}
	return false;
}