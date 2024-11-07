#include "GameObject.h"

GameObject::GameObject()
	:	transform(Transform::defaultTransform()), 
		parent(nullptr),
		tag(0)
{}

GameObject::~GameObject() {}

//draw() should always be overridden in a child class if that child is visible
void GameObject::draw(int renderpass) {}

//update should always be overridden in a child class if that class has dynamic behaviour
void GameObject::update(float deltaTime) {}

int GameObject::getRenderPasses()
{
	return static_cast<int>(materials.size());
}

void GameObject::activateMaterial(int renderPass, Camera* camera, Light light)
{
	materials[renderPass]->use(camera, light);
}

void GameObject::onCollision(GameObject* otherObject)
{
	//do nothing by default
}

void GameObject::removeFromParentsChildren()
{
	if (parent)
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

GameObject* GameObject::findChildByName(const std::string_view childName) const
{
	for (auto& child : children)
	{
		if (child->name == childName)
		{
			return child.get();
		}
		else
		{
			//depth first search for any descendant matching the name
			GameObject* furtherDescendant = child->findChildByName(childName);
			if (furtherDescendant)
			{
				return furtherDescendant;
			}
		}
	}
	return nullptr;
}

const GameObject* GameObject::getRoot() const
{
	if (parent)
	{
		return parent->getRoot();
	}
	return this;
}

bool GameObject::isAncestorOf(const GameObject* other) const
{
	return other->isDescendantOf(this);
}

bool GameObject::isDescendantOf(const GameObject* other) const
{
	if (parent)
	{
		if (parent == other)
		{
			return true;
		}
		return parent->isDescendantOf(other);
	}
	return false;
}

void GameObject::markForDeletion()
{
	flaggedForDeletion = true;
	for (auto& child : children)
	{
		child->markForDeletion();
	}
	children.clear();
}

GameObject* GameObject::checkCollision(Collider* otherCollider, Transform& otherTransform)
{
	//We assume otherObject meets the criteria for collision checks
	if (enabled)
	{
		if (collider)
		{
			Transform effectiveTransform = computeEffectiveTransform();
			if (otherCollider->quickTest(*(collider.get()), effectiveTransform, otherTransform))
			{
				//oh no, they're close to each other. Do the more computationally expensive test
				if (otherCollider->slowTest(collider.get(), effectiveTransform, otherTransform))
				{
					return this;
				}
			}
		}
		for (auto& child : children)
		{
			GameObject* collidingChild = child->checkCollision(otherCollider, otherTransform);
			if (collidingChild)
			{
				return collidingChild;
			}
		}
	}
	return nullptr;
}
