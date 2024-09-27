#pragma once

#include <forward_list>
#include <memory>
#include <mutex>
#include <vector>

#include "Camera.h"
#include "GameObject.h"
#include "GameObjectReference.h"
#include "Level.h"
#include "Light.h"

struct ObjectAndReferencesGroup
{
	std::shared_ptr<GameObject> gameObject;
	std::vector<std::shared_ptr<GameObjectReference>> references;
};

//Manages game objects present in the environment
class Scene
{
private:
	Camera* mainCamera;
	std::forward_list<std::shared_ptr<ObjectAndReferencesGroup>> objectsInScene;
	std::mutex sceneObjectsLock;
public:
	std::vector<Light> lights;	//TODO: Currently drawing handles only one light (no more, no fewer)
	int totalObjects;
	int drawCount;
	int materialActivations;
	Scene(Camera* mainCamera);
	~Scene();
	//void setCullList();
	void draw();
	void addObjectReference(std::shared_ptr<GameObjectReference> reference);	//not thread safe
	void addObjectReferenceBatch(std::vector<std::shared_ptr<GameObjectReference>> batch);	//thread safe
	void deleteReferencesByTag(int tag);
	void clearScene();	//Removes all objects from the scene
	void replaceSceneContentWithLevel(Level* level);	//Removes all objects from the scene, then replaces them with the contents of the level
	void addLevelToSceneAdditive(Level* level);		//Adds the contents of the level to the scene without removing current objects
	GameObjectReference* rayCast(glm::vec3 origin, glm::vec3 direction, glm::vec3& hitLocation) const;	//test a ray against all colliding objects within the scene
};