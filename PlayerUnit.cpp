#include "PlayerUnit.h"

PlayerUnit::PlayerUnit(GraphicsResourceManager* resourceManager)
{
	//Probably need to place a default material here
	Mesh* cube = resourceManager->loadMesh("test_cube");
	materials.push_back(resourceManager->loadMaterial("DefaultRed"));

	StaticMesh* pillar = new StaticMesh(cube, materials[0]);
	Transform pillarTransform(glm::vec3(-0.25f, 0.0f, -0.25f), glm::identity<glm::mat4>(), glm::vec3(0.5f, 0.8f, 0.5f));
	children.push_back(std::tuple(pillar, pillarTransform));

	StaticMesh* flatBase = new StaticMesh(cube, materials[0]);
	Transform baseTransform(glm::vec3(-0.4f, 0.0f, -0.4f), glm::identity<glm::mat4>(), glm::vec3(0.8f, 0.1f, 0.8f));
	children.push_back(std::tuple(flatBase, baseTransform));
}

void PlayerUnit::draw(int renderPass, Transform transform)
{
	//Because this is a compound object where we just use child objects for all visible components, no action is required here
}