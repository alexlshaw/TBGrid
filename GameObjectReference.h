#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "Camera.h"
#include "GameObject.h"
#include "Light.h"
#include "Transform.h"


class GameObjectReference
{
private:
	GameObjectReference* parent;	//Ownership: a child object has no responsibility or ownership of its parent, so we just use a raw pointer
	bool selfDestructing = false;	//When we delete a ref, we want to remove it from its parent's list of children, but if a parent is being destroyed, we don't want its children to mess with it
	void removeFromParentsChildren();
	void constructChildReferences();
	Transform computeEffectiveTransform();
	void addChild(GameObjectReference* child);
public:
	std::vector<std::shared_ptr<GameObjectReference>> children;	//Ownership: A parent object has shared responsibility (with the scene/level) of any of its child objects
	bool enabled = true;
	bool cullingFlag = false;
	int cullingPlaneCache = -1;
	int tag;
	GameObjectReference(std::shared_ptr<GameObject> target);
	GameObjectReference(std::shared_ptr<GameObject> target, int tag);
	GameObjectReference(std::shared_ptr<GameObject> target, Transform transform);
	GameObjectReference(std::shared_ptr<GameObject> target, Transform transform, int tag);
	~GameObjectReference();
	std::shared_ptr<GameObject> base;
	Transform transform;
	void draw(int renderPass);
	GameObjectReference* getParent();
	void setParent(GameObjectReference* newParent);
};