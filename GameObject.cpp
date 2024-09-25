#include "GameObject.h"

GameObject::~GameObject()
{
	if (collider != nullptr)
	{
		delete collider;
	}
	//A gameobject is responsible for destroying its children
	for (auto& child : children)
	{
		delete std::get<0>(child);
	}
}

int GameObject::getRenderPasses()
{
	return static_cast<int>(materials.size());
}

void GameObject::activateMaterial(int renderPass, Camera* camera, Light light)
{
	materials[renderPass]->use(camera, light);
}