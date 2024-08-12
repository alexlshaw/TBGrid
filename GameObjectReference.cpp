#include "GameObjectReference.h"

GameObjectReference::GameObjectReference(GameObject* target)
	: base(target), transform(Transform()), tag(-1), parent(nullptr) {}

GameObjectReference::GameObjectReference(GameObject* target, int tag)
	: base(target), transform(Transform()), tag(tag), parent(nullptr) {}

GameObjectReference::GameObjectReference(GameObject* target, Transform transform)
	: base(target), transform(transform), tag(-1), parent(nullptr) {}

GameObjectReference::GameObjectReference(GameObject* target, Transform transform, int tag)
	: base(target), transform(transform), tag(tag), parent(nullptr) {}

GameObjectReference::~GameObjectReference() 
{
	selfDestructing = true;
	//A reference is responsible for the destruction of its children
	for (auto& child : children)
	{
		delete child;
	}
	children.clear();
	if (parent != nullptr && !parent->selfDestructing)
	{
		removeFromParentsChildren();
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
	base->draw(renderPass, transform);
	
	for (auto& child : children)
	{
		child->drawAsChild(renderPass, transform);
	}
}

void GameObjectReference::drawAsChild(int renderPass, Transform parentsTransform)
{
	Transform effectiveTransform = parentsTransform.stackTransforms(this->transform);
	base->draw(renderPass, effectiveTransform);
	for (auto& child : children)
	{
		child->drawAsChild(renderPass, effectiveTransform);
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