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

//Base class representing an object that exists within the scene
class GameObject
{
private:
protected:
	//TODO: materials probably shouldn't be a part of gameobject, but rather of the thing that is actually drawn (e.g. the mesh)
	std::vector<Material*> materials;
	GameObject* parent; //Ownership: a child object has no responsibility or ownership of its parent, so we just use a raw pointer
	void removeFromParentsChildren();
	Transform computeEffectiveTransform();
public:
	GameObject();
	virtual ~GameObject();
	virtual void draw(int renderPass);	
	virtual int getRenderPasses();
	virtual void activateMaterial(int renderPass, Camera* camera, Light light);
	std::vector<std::shared_ptr<GameObject>> children;	//Ownership: A parent object has shared responsibility (with the scene/level) of any of its child objects
	std::string name = "";
	int tag;
	bool enabled = true;
	std::unique_ptr<Collider> collider = nullptr;
	Transform transform;
	void addChild(GameObject* child);
	void addChild(std::shared_ptr<GameObject> child);
	GameObject* getParent() const;
	void setParent(GameObject* parent);
	GameObject* findChildByName(std::string childName);
};