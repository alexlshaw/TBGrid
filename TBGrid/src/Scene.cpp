#include "Scene.h"
#include <algorithm>

Scene::Scene(Camera* mainCamera)
	:	mainCamera(mainCamera)
{}

Scene::~Scene()
{
	clearScene();
}

void Scene::update(float deltaTime)
{
	//TODO: Is there a meaningful performance cost to calling update() on static objects with an empty update() function?
	for (auto& object : objectsInScene)
	{
		updateObjectAndDescendants(object, deltaTime);
	}
	//clean up anything flagged for removal
	objectsInScene.erase(std::remove_if(objectsInScene.begin(), objectsInScene.end(), 
		[](std::shared_ptr<GameObject> obj) {return obj->flaggedForDeletion; }), 
		objectsInScene.end());
}

void Scene::updateObjectAndDescendants(std::shared_ptr<GameObject> object, const float deltaTime)
{
	if (object->enabled)
	{
		object->update(deltaTime);
		for (auto& child : object->children)
		{
			updateObjectAndDescendants(child, deltaTime);
		}
	}
}

void Scene::collisionUpdate(float deltaTime)
{
	for (auto& object : objectsInScene)
	{
		GameObject* other = testCollisionForObjectAndDescendants(object);
		if (other)
		{
			object->onCollision(other);
			other->onCollision(&(*object));
		}
	}
}

GameObject* Scene::testCollisionForObjectAndDescendants(std::shared_ptr<GameObject> object)
{
	GameObject* other = nullptr;
	//if the object isn't enabled, we don't bother testing it or any children
	if (object->enabled)
	{
		if (object->dynamic && object->collider != nullptr)
		{
			//meets test criteria, check it
			other = testObjectCollision(object);
		}
		if (!other)
		{
			//object either did not collide or was not valid for tests, check its children
			for (auto& child : object->children)
			{
				other = testCollisionForObjectAndDescendants(child);
				//if a collision happens on one child, no need to test other children
				if (other)
				{
					return other;
				}
			}
		}
	}
	return other;
}

void Scene::addObject(std::shared_ptr<GameObject> object)
{
	objectsInScene.push_back(object);
}


void Scene::deleteObject(std::shared_ptr<GameObject> toDelete)
{
	//first recursively destroy the child objects
	for (int i = 0; i < static_cast<int>(toDelete->children.size()); i++)
	{
		deleteObject(toDelete->children[i]);
	}
	toDelete->children.clear();
	//then remove the object itself
	toDelete->flaggedForDeletion = true;
}

void Scene::addObjectBatch(std::vector<std::shared_ptr<GameObject>> batch)
{
	//TODO: Optimise this by pre-grouping by base object, so that we don't have to repeat the object group test in addObjectReference every single time
	for (unsigned int i = 0; i < batch.size(); i++)
	{
		addObject(batch[i]);
	}
}

void Scene::clearScene()
{
	//remove all objects from scene, trusting shared_ptr reference counting to destroy them
	pointLights.clear();
	objectsInScene.clear();
}

void Scene::replaceSceneContentWithLevel(Level* level)
{
	//delete old content
	clearScene();
	//add new content
	objectsInScene.reserve(level->objects.size() + RUNTIME_OBJECT_RESERVATION_BUFFER);
	addObjectBatch(level->objects);
	//add lights
	if (level->sun.direction != glm::vec4{})
	{
		sun = std::make_shared<DirectionalLight>(level->sun);
	}
	for (auto& light : level->lights)
	{
		pointLights.push_back(light);
	}
	level->addedToScene = true;
}

void Scene::addLevelToSceneAdditive(Level* level)
{
	objectsInScene.reserve(objectsInScene.size() + level->objects.size() + RUNTIME_OBJECT_RESERVATION_BUFFER);
	addObjectBatch(level->objects);
	//add lights
	if (level->sun.direction != glm::vec4{})
	{
		sun = std::make_shared<DirectionalLight>(level->sun);
	}
	for (auto& light : level->lights)
	{
		pointLights.push_back(light);
	}
	level->addedToScene = true;
}

GameObject* Scene::rayCast(glm::vec3 origin, glm::vec3 direction, glm::vec3& hitLocation, const int layerMask) const
{
	//TODO: Currently just iterating over *all* active scene objects. More efficient to do some pre-selection of objects worth testing
	glm::vec3 closestHitLocation(0.0f, 0.0f, 0.0f);
	float closestHitDistance = 999999999.0f;	//arbitrarily large
	GameObject* closestHitObject = nullptr;
	//test all of the objects
	for (auto& obj : objectsInScene)
	{
		glm::vec3 hit;
		std::shared_ptr<GameObject> hitObject = testRayAgainstObjectAndDescendants(obj, origin, direction, hit, layerMask);
		if (hitObject)
		{
			//hitObject will either be the object we just tested, or one of its descendants
			//Now test if it is the closest thing we have hit so far
			float hitDistance = glm::length(origin - hit);
			if (closestHitObject == nullptr || hitDistance < closestHitDistance)
			{
				closestHitObject = hitObject.get();
				closestHitDistance = hitDistance;
				closestHitLocation = hit;
			}
		}
	}
	//set our return values based on what we've found
	hitLocation = closestHitLocation;
	return closestHitObject;
}

std::shared_ptr<GameObject> Scene::testRayAgainstObjectAndDescendants(std::shared_ptr<GameObject> object, const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3& hitLocation, const int layerMask) const
{
	if (object->enabled)
	{
		//does it have a collider and is the collider on the layer we want to test?
		if (object->collider && (object->collider->layer & layerMask))
		{
			Transform effectiveTransform = object->computeEffectiveTransform();
			if (object->collider->testRay(rayOrigin, rayDirection, effectiveTransform, hitLocation))
			{
				//we have a hit
				return object;
			}
		}
		else
		{
			//we only test children if the parent does not have its own collider
			for (auto& child : object->children)
			{
				std::shared_ptr<GameObject> collidingChild = testRayAgainstObjectAndDescendants(child, rayOrigin, rayDirection, hitLocation, layerMask);
				if (collidingChild)
				{
					return collidingChild;
				}
			}
		}
	}
	return nullptr;
}

GameObject* Scene::testObjectCollision(std::shared_ptr<GameObject> objectToTest)
{
	if (objectToTest->collider == nullptr)
	{
		return nullptr;	//well that was easy
	}
	//Assuming we don't fail the most basic sanity check above, test against other objects in the scene
	Transform effectiveTransform = objectToTest->computeEffectiveTransform();	//If oTT is a child object, we need to test it's world space position, and we only want to compute that once
	//TODO: Should probably optimise this further than the quickTest() hack
	for (auto& otherObject : objectsInScene)
	{
		//Check if the other object is not a direct ancestor or descendant of our object to test
		if (!otherObject->isAncestorOf(objectToTest.get()) && !objectToTest->isAncestorOf(otherObject.get()))
		{
			GameObject* collidingObject = otherObject->checkCollision(objectToTest->collider.get(), effectiveTransform);
			if (collidingObject)
			{
				//collidingObject is either otherObject, or one of its children
				return collidingObject;
			}
		}
	}
	//if we made it down here, no object has collided
	return nullptr;
}

GameObject* Scene::findObjectByName(const std::string_view objectName) const
{
	for (auto& object : objectsInScene)
	{
		if (object->name == objectName)
		{
			return object.get();
		}
		else
		{
			GameObject* descendant = object->findChildByName(objectName);
			if (descendant)
			{
				return descendant;
			}
		}
	}
	return nullptr;
}

LightBlock Scene::getLights() const
{
	LightBlock lightBlock{};
	if (sun)
	{
		lightBlock.dirLight = *sun;
	}
	for (int i = 0; i < static_cast<int>(pointLights.size()) && i < Lighting::MAX_LIGHT_COUNT; i++)
	{
		lightBlock.pointLights[i] = pointLights[i];
	}
	lightBlock.lightCount = std::min(Lighting::MAX_LIGHT_COUNT, static_cast<int>(pointLights.size()));
	return lightBlock;
}
