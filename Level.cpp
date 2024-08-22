#include "Level.h"

Level::Level(GraphicsResourceManager* resourceManager) 
	: resourceManager(resourceManager) {}

Level::~Level() 
{
	if (!addedToScene)
	{
		//Normally the scene class is responsible for deallocating any object references, since they may be created during gameplay
		//However, when we load a level, there is no guarantee that the level actually gets added to the scene
		//So if the level is never added to a scene, then it has to be responsible for its own gameobject references
		//This is imperfect, as once added to a scene, any object with no references will not be deleted
		//However, if the code for loading a level is generating referenceless objects, something weird is happening anyway
		//TLDR: TODO this should probably use shared_ptr or something
		for (auto& reference : objectReferences)
		{
			delete reference;
		}
		for (auto& object : objects)
		{
			delete object;
		}
	}
}

void Level::buildTestLevel()
{
	//Add lights
	lights.push_back(Light(glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.7f, 0.7f, 0.7f), glm::vec3(), glm::vec3(-1000.0f, 1000.0f, -1000.0f)));
	//Add objects
	Mesh* cube;
	StaticMesh* testStaticMesh;
	Material* defaultMaterial;

	defaultMaterial = resourceManager->loadMaterial("DefaultLit");
	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices;
	MeshTools::addCuboid(&vertices, &indices, glm::vec3(0.5f, 0.5f, 0.5f), 1.0f, 1.0f, 1.0f);
	cube = new Mesh(vertices, indices);
	resourceManager->addMesh("test_cube", cube);
	testStaticMesh = new StaticMesh(cube, defaultMaterial);
	testStaticMesh->collider = new BoxCollider();
	objects.push_back(testStaticMesh);

	for (int x = 0; x < 20; x++)
	{
		for (int z = 0; z < 20; z++)
		{
			Transform t(glm::vec3((float)x, -0.1f, float(z)), glm::identity<glm::mat4>(), glm::vec3(1.0f, 0.1f, 1.0f));
			GameObjectReference* tile = new GameObjectReference(testStaticMesh, t);
			objectReferences.push_back(tile);
		}
	}
}

void Level::loadLevel(std::string levelName)
{
	DEBUG_PRINT("Error: Loading level from file is not yet implemented\n");
}