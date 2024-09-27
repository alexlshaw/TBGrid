#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "Camera.h"
#include "Collider.h"
#include "DebuggingTools.h"
#include "Light.h"
#include "Material.h"
#include "Transform.h"

//Base class representing an object that has references in the world
class GameObject
{
private:
protected:
	//TODO: materials probably shouldn't be a part of gameobject, but rather of the thing that is actually drawn (e.g. the mesh)
	std::vector<Material*> materials;
public:
	GameObject() {};
	virtual ~GameObject();
	virtual void draw(int renderPass, Transform transform) = 0;	//draw() should always be overridden in a child class
	virtual int getRenderPasses();
	virtual void activateMaterial(int renderPass, Camera* camera, Light light);
	std::string name = "";
	bool noCull = false;	//if true, the scene never tries to cull the object if it is outside the camera's frustrum
	std::unique_ptr<Collider> collider = nullptr;

	//GameObject references must be able to have child references (hierarchical scene graph)
	//By extension, when we construct a game object we're going to need to know what children
	//we want to add to its references
	//We're also going to need to know the relative locations of these children
	//So that when we construct the reference we can add the children too
	//Not a huge fan of this approach, although GameObject dictates ref, it feels like a bit of duplication
	std::vector<std::tuple<std::shared_ptr<GameObject>, Transform>> children;
	//This also highlights an issue with the object-ref design, where properties of an object are defined on the gameobject itself, but the references
	//might want to independently track these values so two references of the same object can have different values
};


/*
The conclusion I'm coming to on GameObject & GameObjectRef is that ref exists to solve the wrong problem
The idea is that I want to be able to group mesh/material sets so that I can batch large numbers of identical objects for drawing
But there is also a secondary consideration of being able to rapidly instantiate multiple copies of the same object (e.g. a bullet)
For any objects that is an actor (so has behaviour or tracks more properties than just location, e.g. basically everything that isn't a static mesh)
each property will be unique per instance
*/