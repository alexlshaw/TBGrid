#include "GameObjectReference.h"

GameObjectReference::GameObjectReference(GameObject* target)
	: base(target), transform(Transform()), tag(-1), parent(nullptr) 
{
	constructChildReferences();
}

GameObjectReference::GameObjectReference(GameObject* target, int tag)
	: base(target), transform(Transform()), tag(tag), parent(nullptr) 
{
	constructChildReferences();
}

GameObjectReference::GameObjectReference(GameObject* target, Transform transform)
	: base(target), transform(transform), tag(-1), parent(nullptr)
{
	constructChildReferences();
}

GameObjectReference::GameObjectReference(GameObject* target, Transform transform, int tag)
	: base(target), transform(transform), tag(tag), parent(nullptr)
{
	constructChildReferences();
}

GameObjectReference::~GameObjectReference() 
{
	//THE SCENE IS NOW RESPONSIBLE FOR DELETING CHILD REFERENCES ON SCENE CLEANUP
	//TODO: But we probably need something to remove children from the scene if we delete a singular reference
	//tldr: shared_ptr
}

Transform GameObjectReference::computeEffectiveTransform()
{
	//if we have no parent, our world space transform is just our transform
	if (parent == nullptr)
	{
		return transform;
	}
	//if we do have a transform, recursively calculate our effective transform based on our parental heirarchy
	return parent->computeEffectiveTransform().stackTransforms(transform);
}

void GameObjectReference::constructChildReferences()
{
	//Our base game object may specify child objects, we have to construct references for them
	for (auto& child : base->children)
	{
		//tuple is (baseObject, relative transform)
		GameObjectReference* childRef = new GameObjectReference(std::get<0>(child), std::get<1>(child));
		childRef->parent = this;
		children.push_back(childRef);
	}
}

void GameObjectReference::removeFromParentsChildren()
{
	auto it = std::find(parent->children.begin(), parent->children.end(), this);
	if (it != parent->children.end())
	{
		parent->children.erase(it);
	}
	else
	{
		DEBUG_PRINT("Error attempting to remove child objectRef from parent: child ref not present in parent's list of children\n");
	}
}

void GameObjectReference::draw(int renderPass)
{
	//We only draw the object if it's enabled
	if (enabled)
	{
		base->draw(renderPass, computeEffectiveTransform());
	}
}

GameObjectReference* GameObjectReference::getParent()
{
	return parent;
}

void GameObjectReference::setParent(GameObjectReference* newParent)
{
	if (parent == nullptr)
	{
		//easy, just set it and add it
		parent = newParent;
		parent->children.push_back(this);
	}
	else
	{
		//first remove from old parent
		removeFromParentsChildren();
		//then set new parent
		parent = newParent;
		parent->children.push_back(this);
	}
}