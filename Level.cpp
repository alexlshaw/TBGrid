#include "Level.h"

Level::Level(GraphicsResourceManager* resourceManager) 
	: resourceManager(resourceManager) {}

Level::~Level() 
{
	//We trust the shared_ptr to manage any deletions, so no action required here
}

void Level::buildTestLevel()
{
	//Add lights
	lights.push_back(Light(glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(), glm::vec3(-1000.0f, 1000.0f, -1000.0f)));
	//Add objects
	Mesh* cube;
	Material* defaultMaterial;

	defaultMaterial = resourceManager->loadMaterial("DefaultLit");
	cube = resourceManager->loadMesh("unit_cube");
	std::shared_ptr<StaticMesh> testStaticMesh = std::make_shared<StaticMesh>(cube, defaultMaterial);
	testStaticMesh->name = "Level Tile";
	testStaticMesh->collider = std::make_unique<BoxCollider>();
	objects.push_back(testStaticMesh);

	for (int x = 0; x < 20; x++)
	{
		for (int z = 0; z < 20; z++)
		{
			Transform t(glm::vec3((float)x, -0.1f, float(z)), glm::identity<glm::mat4>(), glm::vec3(1.0f, 0.1f, 1.0f));
			addReference(testStaticMesh, t);
		}
	}

	//Create a unit for the player to control
	std::shared_ptr<PlayerUnit> playerUnit = std::make_shared<PlayerUnit>(resourceManager);
	objects.push_back(playerUnit);
	Transform unitTransform(glm::vec3(0.5f, 0.0f, 0.5f), glm::identity<glm::mat4>(), glm::vec3(1.0f, 1.0f, 1.0f));
	addReference(playerUnit, unitTransform);

	//Create a line renderer for testing
	std::shared_ptr<LineRenderer> lineRenderer = std::make_shared<LineRenderer>(resourceManager, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	objects.push_back(lineRenderer);
	std::vector<glm::vec3> linePoints = { 
		glm::vec3(0.0f, 0.0f, 0.0f), 
		glm::vec3(-1.0f, 0.0f, -1.0f) 
	};
	lineRenderer->generateSegmentsFromPoints(linePoints);
	addReference(lineRenderer, Transform::defaultTransform());
}

void Level::loadLevel(std::string levelName)
{
	DEBUG_PRINT("Error: Loading level from file is not yet implemented\n");
}

void Level::addReference(std::shared_ptr<GameObject> object, Transform transform)
{
	std::shared_ptr<GameObjectReference> ref = std::make_shared<GameObjectReference>(object, transform);
	objectReferences.push_back(ref);
}