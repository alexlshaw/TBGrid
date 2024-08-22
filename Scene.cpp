#include "Scene.h"

Scene::Scene(Camera* mainCamera)
{
	this->mainCamera = mainCamera;
	drawCount = 0;
	materialActivations = 0;
	totalObjects = 0;
}

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
	for (auto& group : objectsInScene)
	{
		int passes = group->gameObject->getRenderPasses();
		for (int pass = 0; pass < passes; pass++)
		{
			group->gameObject->activateMaterial(pass, mainCamera, lights[0]);
			materialActivations++;
			for (unsigned int i = 0; i < group->references.size(); i++)
			{
				GameObjectReference* reference = group->references[i];
				if (!reference->cullingFlag)
				{
					reference->draw(pass);
					drawCount++;
				}
			}
		}
	}
	sceneObjectsLock.unlock();
}

void Scene::addObjectReference(GameObjectReference* reference)
{
	//check to see if there is an object group for this object already
	//need to set this up so that it is sorted for quick lookups (Use map?)
	totalObjects++;
	GameObject* baseObject = reference->base;
	for (auto& group : objectsInScene)
	{
		if (group->gameObject == baseObject)
		{
			group->references.push_back(reference);
			return;
		}
	}
	//if we've made it down to here, there was no object group for this base object
	ObjectAndReferencesGroup* group = new ObjectAndReferencesGroup;
	group->gameObject = baseObject;
	group->references.push_back(reference);
	objectsInScene.push_front(group);
}

void Scene::deleteReferencesByTag(int tag)
{
	sceneObjectsLock.lock();
	for (auto& group : objectsInScene)
	{
		for (auto i = group->references.begin(); i != group->references.end();)
		{
			GameObjectReference* p = *i;
			if (p->tag == tag)
			{
				totalObjects--;
				i = group->references.erase(i);
				delete p;
			}
			else
			{
				i++;
			}
		}
	}
	sceneObjectsLock.unlock();
}

void Scene::addObjectReferenceBatch(std::vector<GameObjectReference*> batch)
{
	//TODO: Optimise this by pre-grouping by base object, so that we don't have to repeat the object group test in addObjectReference every single time
	sceneObjectsLock.lock();
	for (unsigned int i = 0; i < batch.size(); i++)
	{
		addObjectReference(batch[i]);
	}
	sceneObjectsLock.unlock();
}

void Scene::clearScene()
{
	lights.clear();
	//dispose of the scene object references and then the reference group object.
	//Important note: any class that creates an object or reference but doesn't add it to a scene has to be responsible for that object's destruction
	for (auto& group : objectsInScene)
	{
		for (auto& reference : group->references)
		{
			delete reference;
		}
		delete group->gameObject;
		delete group;
	}
}

void Scene::replaceSceneContentWithLevel(Level* level)
{
	//delete old content
	clearScene();
	//add new content
	addObjectReferenceBatch(level->objectReferences);
	for (auto& light : level->lights)
	{
		lights.push_back(light);
	}
	level->addedToScene = true;
}

void Scene::addLevelToSceneAdditive(Level* level)
{
	addObjectReferenceBatch(level->objectReferences);
	for (auto& light : level->lights)
	{
		lights.push_back(light);
	}
	level->addedToScene = true;
}

GameObjectReference* Scene::rayCast(glm::vec3 origin, glm::vec3 direction, glm::vec3& hitLocation)
{
	//TODO: Currently just iterating over *all* scene objects. More efficient to do some pre-selection of objects worth testing
	glm::vec3 closestHitLocation(0.0f, 0.0f, 0.0f);
	float closestHitDistance = 999999999.0f;	//arbitrarily large
	GameObjectReference* closestHitObject = nullptr;
	for (auto& group : objectsInScene)
	{
		if (group->gameObject->collider != nullptr)
		{
			//the object does have a collider, we need to test all references associated with the object
			for (auto& reference : group->references)
			{
				//TODO: we also need to check sub-objects, but we'll deal with that later
				glm::vec3 hit;
				if (reference->base->collider->testRay(origin, direction, reference->transform, hit))
				{
					//we have a hit, is it the closest we have so far?
					float hitDistance = glm::length(origin - hit);
					if (closestHitObject == nullptr || hitDistance < closestHitDistance)
					{
						//this hit is closer, we have a new hit object
						closestHitObject = reference;
						closestHitDistance = hitDistance;
						closestHitLocation = hit;
					}
				}
			}
		}
	}
	//set our return values based on what we've found
	hitLocation = closestHitLocation;
	return closestHitObject;
}