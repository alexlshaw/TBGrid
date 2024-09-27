#include "GameObjectReference.h"

GameObjectReference::GameObjectReference(std::shared_ptr<GameObject> target)
	: base(target), transform(Transform()), tag(-1), parent(nullptr) 
{
	constructChildReferences();
}

GameObjectReference::GameObjectReference(std::shared_ptr<GameObject> target, int tag)
	: base(target), transform(Transform()), tag(tag), parent(nullptr) 
{
	constructChildReferences();
}

GameObjectReference::GameObjectReference(std::shared_ptr<GameObject> target, Transform transform)
	: base(target), transform(transform), tag(-1), parent(nullptr)
{
	constructChildReferences();
}

GameObjectReference::GameObjectReference(std::shared_ptr<GameObject> target, Transform transform, int tag)
	: base(target), transform(transform), tag(tag), parent(nullptr)
{
	constructChildReferences();
}

GameObjectReference::~GameObjectReference() 
{
	//We trust our use of std::shared_ptr to handle all deletion, no action required here
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
		//tuple is (shared_ptr<baseObject>, local transform)
		auto go = std::get<0>(child);
		Transform transform = std::get<1>(child);
		std::shared_ptr<GameObjectReference> childRef = std::make_shared<GameObjectReference>(go, transform);
		childRef->parent = this;
		children.push_back(childRef);
	}
}

void GameObjectReference::removeFromParentsChildren()
{
	//Search parent's children for this ref
	auto it = std::find_if(parent->children.begin(), parent->children.end(), 
		[this](const std::shared_ptr<GameObjectReference> r) 
		{
			return r.get() == this; 
		});
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
		parent->addChild(this);
	}
	else
	{
		//first remove from old parent
		removeFromParentsChildren();
		//then set new parent
		parent = newParent;
		parent->addChild(this);
	}
}

void GameObjectReference::addChild(GameObjectReference* child)
{
	children.push_back(std::shared_ptr<GameObjectReference>(child));
}