#pragma once

#include <algorithm>
#include <vector>

#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"


class GameObjectReference
{
private:
	GameObjectReference* parent;
	bool selfDestructing = false;	//When we delete a ref, we want to remove it from its parent's list of children, but if a parent is being destroyed, we don't want its children to mess with it
	void removeFromParentsChildren();
public:
	std::vector<GameObjectReference*> children;
	
	bool cullingFlag = false;
	int cullingPlaneCache = -1;
	int tag;
	GameObjectReference(GameObject* target);
	GameObjectReference(GameObject* target, int tag);
	GameObjectReference(GameObject* target, Transform transform);
	GameObjectReference(GameObject* target, Transform transform, int tag);
	~GameObjectReference();
	GameObject* base;
	Transform transform;
	void draw(int renderPass);	//draw the game object directly. used when game object is an orphan
	void drawAsChild(int renderPass, Transform parentsTransform);	//used when the game object is the child of another object
	GameObjectReference* getParent();
	void setParent(GameObjectReference* newParent);
};