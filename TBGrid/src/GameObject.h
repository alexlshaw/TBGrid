#pragma once

#include <memory>
#include <string>
#include <string_view>
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
public:
	GameObject();
	virtual ~GameObject();
	virtual void draw(int renderPass);
	virtual void update(float deltaTime);
	virtual int getRenderPasses();
	virtual void activateMaterial(int renderPass, Camera* camera, Light light);
	virtual void onCollision(GameObject* otherObject);
	std::vector<std::shared_ptr<GameObject>> children;	//Ownership: A parent object has shared responsibility (with the scene/level) of any of its child objects
	std::string name = "";
	int tag;
	bool enabled = true;
	bool dynamic = false;	//Flag for the scene to indicate the object changes frequently and must be checked every frame
	bool flaggedForDeletion = false;
	std::unique_ptr<Collider> collider = nullptr;
	Transform transform;
	Transform computeEffectiveTransform();	//Compute the final transform after considering the transforms of parent/ancestor objects
	void addChild(GameObject* child);
	void addChild(std::shared_ptr<GameObject> child);
	GameObject* getParent() const;
	void setParent(GameObject* parent);
	GameObject* findChildByName(const std::string_view childName) const;	//Searches children (recursively) for one matching the given name
	const GameObject* getRoot() const;
	bool isAncestorOf(const GameObject* other) const;
	bool isDescendantOf(const GameObject* other) const;
	void markForDeletion();	//Same as calling Scene::deleteObject(this) -> marks the object and all child objects for deletion
	GameObject* checkCollision(Collider* otherCollider, Transform& otherTransform);
};