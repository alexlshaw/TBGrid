#pragma once

#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"


class GameObjectReference
{
private:
public:
	bool cullingFlag = false;
	int cullingPlaneCache = -1;
	int tag;
	GameObjectReference(GameObject* target);
	GameObjectReference(GameObject* target, int tag);
	~GameObjectReference();
	GameObject* base;
	Transform* transform;
	void draw(int renderPass);
};