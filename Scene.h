#pragma once

#include <forward_list>
#include <mutex>
#include <vector>

#include "Camera.h"
#include "GameObject.h"
#include "GameObjectReference.h"
#include "Level.h"
#include "Light.h"

struct ObjectAndReferencesGroup
{
	GameObject* gameObject;
	std::vector<GameObjectReference*> references;
};

//Manages game objects present in the environment
class Scene
{
private:
	Camera* mainCamera;
	std::forward_list<ObjectAndReferencesGroup*> objectsInScene;
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
	void addObjectReference(GameObjectReference* reference);	//not thread safe
	void addObjectReferenceBatch(std::vector<GameObjectReference*> batch);	//thread safe
	void deleteReferencesByTag(int tag);
	void clearScene();	//Removes all objects from the scene
	void replaceSceneContentWithLevel(Level* level);	//Removes all objects from the scene, then replaces them with the contents of the level
	void addLevelToSceneAdditive(Level* level);		//Adds the contents of the level to the scene without removing current objects
};