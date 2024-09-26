#include "PlayerUnit.h"

PlayerUnit::PlayerUnit(GraphicsResourceManager* resourceManager)
{
	name = "PlayerUnit";
	//Probably need to place a default material here
	Mesh* cube = resourceManager->loadMesh("unit_cube");
	Material* defaultRed = resourceManager->loadMaterial("DefaultRed");
	collider = new BoxCollider();

	StaticMesh* pillar = new StaticMesh(cube, defaultRed);
	pillar->name = "Player Unit pillar";
	Transform pillarTransform(glm::vec3(-0.25f, 0.0f, -0.25f), glm::identity<glm::mat4>(), glm::vec3(0.5f, 0.8f, 0.5f));
	children.push_back(std::tuple(pillar, pillarTransform));

	StaticMesh* flatBase = new StaticMesh(cube, defaultRed);
	flatBase->name = "Player Unit base";
	Transform baseTransform(glm::vec3(-0.4f, 0.0f, -0.4f), glm::identity<glm::mat4>(), glm::vec3(0.8f, 0.1f, 0.8f));
	children.push_back(std::tuple(flatBase, baseTransform));
	

	//Create the selection plane
	Mesh* plane = resourceManager->loadMesh("unit_plane");
	Material* selectionMaterial = resourceManager->loadMaterial("SelectionIndicator");
	selectedIndicator = new StaticMesh(plane, selectionMaterial);
	selectedIndicator->name = "Player unit selection indicator";
	Transform selectionTransform(glm::vec3(-0.5f, 0.025f, -0.5f), glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	children.push_back(std::tuple(selectedIndicator, selectionTransform));
}

void PlayerUnit::draw(int renderPass, Transform transform)
{
	//Because this is a compound object where we just use child objects for all visible components, no action is required here
}