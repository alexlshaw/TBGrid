#include "GameObject.h"

GameObject::~GameObject()
{
	if (collider != nullptr)
	{
		delete collider;
	}
	//The scene is responsible for deleting child objects on scene cleanup
	//TODO: but if we are deleting a gameobject without cleaning up the whole scene, we probably need to make the gameobejct responsible for it
	//TLDR: shared_ptr
}

int GameObject::getRenderPasses()
{
	return static_cast<int>(materials.size());
}

void GameObject::activateMaterial(int renderPass, Camera* camera, Light light)
{
	materials[renderPass]->use(camera, light);
}