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

//void Scene::setCullList()
//{
//	mainCamera->updateFrustrum();
//	sceneObjectsLock.lock();
//	for (auto& group : objectsInScene)
//	{
//		float drawDistance = VIEW_SQUARED * group->gameObject->drawDistanceScale;
//		for (unsigned int i = 0; i < group->references.size(); i++)
//		{
//			GameObjectReference* reference = group->references[i];
//			//first check if the object is too far away
//			glm::vec3 refPos = reference->transform->getPosition();
//			glm::vec3 d = mainCamera->transform.getPosition() - refPos;
//			if ((d.x * d.x + d.z * d.z) > drawDistance)
//			{
//				reference->cullingPlaneCache = -1;
//				reference->cullingFlag = true;
//			}
//			else
//			{
//				//the object was not too far away, check if it is in the frustrum
//				int failurePlane = reference->cullingPlaneCache;
//				int inFrustrum = mainCamera->frustrum.sphereInFrustrum(refPos, 10.0f, failurePlane);
//				if (inFrustrum == OUTSIDE)
//				{
//					reference->cullingPlaneCache = failurePlane;
//					reference->cullingFlag = true;
//				}
//				else
//				{
//					reference->cullingPlaneCache = -1;
//					reference->cullingFlag = false;
//				}
//			}
//		}
//	}
//	sceneObjectsLock.unlock();
//}

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
	//TODO: Is there a performance cost to calling update() on static objects with an empty update() function?
	for (auto& object : objectsInScene)
	{
		object->update(deltaTime);
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

void Scene::deleteObjectsByTag(int tag)
{
	sceneObjectsLock.lock();
	for (auto it = objectsInScene.begin(); it != objectsInScene.end(); /*No increment*/)
	{
		std::shared_ptr<GameObject> p = *it;
		if (p->tag == tag)
		{
			it = objectsInScene.erase(it);
		}
		else
		{
			++it;
		}
	}
	sceneObjectsLock.unlock();
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