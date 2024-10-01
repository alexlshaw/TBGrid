#include "GameObject.h"

GameObject::GameObject()
	:	transform(Transform::defaultTransform()), 
		parent(nullptr),
		tag(0)
{}

GameObject::~GameObject() {}

int GameObject::getRenderPasses()
{
	return static_cast<int>(materials.size());
}

void GameObject::activateMaterial(int renderPass, Camera* camera, Light light)
{
	materials[renderPass]->use(camera, light);
}

void GameObject::removeFromParentsChildren()
{
	//Search parent's children for this object
	auto it = std::find_if(parent->children.begin(), parent->children.end(),
		[this](const std::shared_ptr<GameObject> obj)
		{
			return obj.get() == this;
		});
	if (it != parent->children.end())
	{
		parent->children.erase(it);
	}
	else
	{
		DEBUG_PRINT("Error attempting to remove child GameObject from parent: child not present in parent's list of children\n");
	}
}

Transform GameObject::computeEffectiveTransform()
{
	//if we have no parent, our world space transform is just our transform
	if (parent == nullptr)
	{
		return transform;
	}
	//if we do have a transform, recursively calculate our effective transform based on our parental heirarchy
	return parent->computeEffectiveTransform().stackTransforms(transform);
}

void GameObject::addChild(GameObject* child)
{
	//We need this check because the setParent function also calls the addChild function
	//So this is our infinite recursion check
	if (child->getParent() != this)
	{
		child->setParent(this);
		//Now add it to the list of children
		children.push_back(std::shared_ptr<GameObject>(child));
	}
}

void GameObject::addChild(std::shared_ptr<GameObject> child)
{
	//We need this check because the setParent function also calls the addChild function
	//So this is our infinite recursion check
	if (child->getParent() != this)
	{
		child->setParent(this);
	}
	//Couldn't find the child object already in children, add it to the list
	children.push_back(child);
}

GameObject* GameObject::getParent() const
{
	return parent;
}

void GameObject::setParent(GameObject* newParent)
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