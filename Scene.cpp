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
				GameObjectReference* reference = group->references[i].get();
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

void Scene::addObjectReference(std::shared_ptr<GameObjectReference> reference)
{
	//check to see if there is an object group for this object already
	//need to set this up so that it is sorted for quick lookups (Use map?)
	totalObjects++;
	GameObject* baseObject = reference->base.get();
	
	//sceneObjectsLock.lock();	//TODO: why does enabling thread locking on this function cause an error (even when using mutex.try_lock())?
	for (auto& group : objectsInScene)
	{
		if (group->gameObject.get() == baseObject)
		{
			group->references.push_back(reference);
			return;
		}
	}
	//if we've made it down to here, there was no object group for this base object
	std::shared_ptr<ObjectAndReferencesGroup> group = std::make_shared<ObjectAndReferencesGroup>();
	group->gameObject = reference->base;
	group->references.push_back(reference);
	objectsInScene.push_front(group);
	//sceneObjectsLock.unlock();
	//Now we need to add child objects as references
	for (auto& child : reference->children)
	{
		addObjectReference(child);
	}
}

void Scene::deleteReferencesByTag(int tag)
{
	sceneObjectsLock.lock();
	for (auto& group : objectsInScene)
	{
		for (auto i = group->references.begin(); i != group->references.end();)
		{
			std::shared_ptr<GameObjectReference> p = *i;
			if (p->tag == tag)
			{
				totalObjects--;
				i = group->references.erase(i);
			}
			else
			{
				i++;
			}
		}
	}
	sceneObjectsLock.unlock();
}

void Scene::addObjectReferenceBatch(std::vector<std::shared_ptr<GameObjectReference>> batch)
{
	//TODO: Optimise this by pre-grouping by base object, so that we don't have to repeat the object group test in addObjectReference every single time
	for (unsigned int i = 0; i < batch.size(); i++)
	{
		addObjectReference(batch[i]);
	}
}

void Scene::clearScene()
{
	sceneObjectsLock.lock();
	//remove all objects from scene, trusting shared_ptr reference counting to destroy them
	lights.clear();
	for (auto& group : objectsInScene)
	{
		group->references.clear();
	}
	objectsInScene.clear();
	sceneObjectsLock.unlock();
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

GameObjectReference* Scene::rayCast(glm::vec3 origin, glm::vec3 direction, glm::vec3& hitLocation) const
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
						closestHitObject = reference.get();
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