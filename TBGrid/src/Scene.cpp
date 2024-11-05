#include "Scene.h"

Scene::Scene(Camera* mainCamera)
	:	mainCamera(mainCamera),
		drawCount(0),
		materialActivations(0)
{}

Scene::~Scene()
{
	clearScene();
}

void Scene::draw()
{
	mainCamera->calculateViewMatrix();
	drawCount = 0;
	materialActivations = 0;
	sceneObjectsLock.lock();
	for (auto& gameObject : objectsInScene)
	{
		if (gameObject->enabled)
		{
			int passes = gameObject->getRenderPasses();
			for (int pass = 0; pass < passes; pass++)
			{
				gameObject->activateMaterial(pass, mainCamera, lights[0]);
				materialActivations++;
				gameObject->draw(pass);
				drawCount++;
			}
		}
	}
	sceneObjectsLock.unlock();
}

void Scene::update(float deltaTime)
{
	//TODO: Is there a meaningful performance cost to calling update() on static objects with an empty update() function?
	for (auto& object : objectsInScene)
	{
		object->update(deltaTime);
	}
	//clean up anything flagged for removal
	objectsInScene.erase(std::remove_if(objectsInScene.begin(), objectsInScene.end(), 
		[](std::shared_ptr<GameObject> obj) {return obj->flaggedForDeletion; }), 
		objectsInScene.end());
}

void Scene::collisionUpdate(float deltaTime)
{
	for (auto& object : objectsInScene)
	{
		if (object->dynamic && object->collider != nullptr)
		{
			GameObject* other = testObjectCollision(object);
			if (other)
			{
				object->onCollision(other);
				other->onCollision(&(*object));
			}
		}
	}
}

void Scene::addObject(std::shared_ptr<GameObject> object)
{
	objectsInScene.push_back(object);
	//Now we need to add child objects
	for (auto& child : object->children)
	{
		addObject(child);
	}
}


void Scene::deleteObject(std::shared_ptr<GameObject> toDelete)
{
	//first recursively destroy the child objects
	for (int i = toDelete->children.size() - 1; i >= 0; i--)
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
	sceneObjectsLock.lock();
	//remove all objects from scene, trusting shared_ptr reference counting to destroy them
	lights.clear();
	objectsInScene.clear();
	sceneObjectsLock.unlock();
}

void Scene::replaceSceneContentWithLevel(Level* level)
{
	//delete old content
	clearScene();
	//add new content
	objectsInScene.reserve(level->objects.size() + RUNTIME_OBJECT_RESERVATION_BUFFER);
	addObjectBatch(level->objects);
	for (auto& light : level->lights)
	{
		lights.push_back(light);
	}
	level->addedToScene = true;
}

void Scene::addLevelToSceneAdditive(Level* level)
{
	objectsInScene.reserve(objectsInScene.size() + level->objects.size() + RUNTIME_OBJECT_RESERVATION_BUFFER);
	addObjectBatch(level->objects);
	for (auto& light : level->lights)
	{
		lights.push_back(light);
	}
	level->addedToScene = true;
}

GameObject* Scene::rayCast(glm::vec3 origin, glm::vec3 direction, glm::vec3& hitLocation) const
{
	//TODO: Currently just iterating over *all* scene objects. More efficient to do some pre-selection of objects worth testing
	glm::vec3 closestHitLocation(0.0f, 0.0f, 0.0f);
	float closestHitDistance = 999999999.0f;	//arbitrarily large
	GameObject* closestHitObject = nullptr;
	//test all of the objects
	for (auto& obj : objectsInScene)
	{
		if (obj->collider != nullptr)
		{
			//TODO: we also need to check sub-objects, but we'll deal with that later
			glm::vec3 hit;
			if (obj->collider->testRay(origin, direction, obj->transform, hit))
			{
				//we have a hit, is it the closest we have so far?
				float hitDistance = glm::length(origin - hit);
				if (closestHitObject == nullptr || hitDistance < closestHitDistance)
				{
					//this hit is closer, we have a new hit object
					closestHitObject = obj.get();
					closestHitDistance = hitDistance;
					closestHitLocation = hit;
				}
			}
		}
	}
	//set our return values based on what we've found
	hitLocation = closestHitLocation;
	return closestHitObject;
}

GameObject* Scene::testObjectCollision(std::shared_ptr<GameObject> objectToTest)
{
	if (objectToTest->collider == nullptr)
	{
		return nullptr;	//well that was easy
	}
	//Assuming we don't fail the most basic sanity check above, test against other objects in the scene
	//TODO: Should probably optimise this further than the quickTest() hack
	for (auto& otherObject : objectsInScene)
	{
		if (otherObject->collider && otherObject.get() != objectToTest.get())
		{
			//otherObject is a relevant candidate for testing
			if (objectToTest->collider->quickTest(*(otherObject->collider.get()), otherObject->transform, objectToTest->transform))
			{
				//oh no, they're close to each other. Do the more computationally expensive test
				if (objectToTest->collider->slowTest(otherObject->collider.get(), otherObject->transform, objectToTest->transform))
				{
					return otherObject.get();
				}
			}
		}
	}
	//if we made it down here, no object has collided
	return nullptr;
}

GameObject* Scene::findObjectByName(std::string objectName)
{
	for (int i = 0; i < objectsInScene.size(); i++)
	{
		if (objectsInScene[i]->name == objectName)
		{
			return objectsInScene[i].get();
		}
	}
	return nullptr;
}