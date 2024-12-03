#pragma once

#include <algorithm>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "Animation.h"
#include "Animator.h"
#include "Camera.h"
#include "DebuggingTools.h"
#include "GameObject.h"
#include "Level.h"
#include "Light.h"

//Manages game objects present in the environment
class Scene
{
private:
	static const int RUNTIME_OBJECT_RESERVATION_BUFFER = 100;	//How much extra space we allocate in our objects list for in-game object instantiation to minimise costly reallocation
	//recursive functions to support activities for objects and all children
	void updateObjectAndDescendants(std::shared_ptr<GameObject> object, const float deltaTime);
	GameObject* testCollisionForObjectAndDescendants(std::shared_ptr<GameObject> object);
	std::shared_ptr<GameObject> testRayAgainstObjectAndDescendants(std::shared_ptr<GameObject> object, const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3& hitLocation, const int layerMask) const;
public:
	AnimatedModel* animModel = nullptr;
	Animation* tauntAnim = nullptr;
	Animator* animator = nullptr;
	Camera* mainCamera;
	std::vector<PointLight> pointLights;
	std::shared_ptr<DirectionalLight> sun = nullptr;
	Scene(Camera* mainCamera);
	~Scene();
	void update(float deltaTime);
	void collisionUpdate(float deltaTime);	//test dynamic objects for collision (might later become a FixedUpdate() sort of thing)
	void addObject(std::shared_ptr<GameObject> object);
	void addObjectBatch(std::vector<std::shared_ptr<GameObject>> batch);
	void deleteObject(std::shared_ptr<GameObject> toDelete);
	void clearScene();	//Removes all objects from the scene
	void replaceSceneContentWithLevel(Level* level);	//Removes all objects from the scene, then replaces them with the contents of the level
	void addLevelToSceneAdditive(Level* level);		//Adds the contents of the level to the scene without removing current objects
	GameObject* rayCast(glm::vec3 origin, glm::vec3 direction, glm::vec3& hitLocation, const int layerMask) const;	//test a ray against all colliding objects within the scene
	GameObject* testObjectCollision(std::shared_ptr<GameObject> objectToTest);
	GameObject* findObjectByName(const std::string_view objectName) const;	//Returns a pointer to the *first* object within the scene with a matching name, or nullptr if not found
	std::vector<std::shared_ptr<GameObject>> objectsInScene;	//TODO: We still want to group this by object type (or rather by material or some similar shared property)
	LightBlock getLights() const;
};