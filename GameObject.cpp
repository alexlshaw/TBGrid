#include "GameObject.h"

GameObject::~GameObject() {}

int GameObject::getRenderPasses()
{
	return static_cast<int>(materials.size());
}

void GameObject::activateMaterial(int renderPass, Camera* camera, Light light)
{
	materials[renderPass]->use(camera, light);
}