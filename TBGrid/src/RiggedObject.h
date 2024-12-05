#pragma once

#include "Animator.h"
#include "GameObject.h"
#include <memory>

//A gameobject that wraps an animated mesh with a skeletal rig and set of animations
//Not to be confused with AnimatedModel, which is the actual rigged mesh, or Animation, which is the set of transformations applied to move the mesh
class RiggedObject : public GameObject
{
private:
	std::shared_ptr<Animator> animator;
public:
	RiggedObject(std::shared_ptr<Animator> animator);
	void draw(size_t meshIndex);	//Doesn't override regular gameobject draw()
	void update(float deltaTime) override;
	size_t getMeshCount();
	void setRigInShader(Shader* shader);
	Material* getMaterial(size_t idx);
};


/*
Important note about how this object is handled during rendering :

The scene tracks these objects in a separate vector, in addition to the main vector
In the main drawObject function, 
	the renderer will attempt to draw the object, but find that it has no materials, or that
	draw(int renderpass) is not implemented
	So it won't draw anything, as planned
Then in the drawAnimatedObject function, it will do it properly

*/