#include "UIElement.h"

UIElement::UIElement(glm::vec2 position)
	:	position(position),
		parent(nullptr)
{
}

void UIElement::drawAll(Shader* shader, const glm::mat4& projection) const
{
	draw(shader, projection);
	for (auto& child : children)
	{
		if (child->enabled)
		{
			child->drawAll(shader, projection);
		}
	}
}

void UIElement::updateAll(float delta)
{
	update(delta);
	for (auto& child : children)
	{
		if (child->enabled)
		{
			child->updateAll(delta);
		}
	}
}

//By default, this does nothing. Override in any child object that cares
void UIElement::draw(Shader* shader, const glm::mat4& projection) const {}

//By default, this does nothing. Override in any child object that cares
void UIElement::update(float delta) {}

void UIElement::addChild(UIElement* child)
{
	//We need this check because the setParent function also calls the addChild function
	//So this is our infinite recursion check
	if (child->getParent() != this)
	{
		child->setParent(this);
		//Now add it to the list of children
		children.push_back(std::shared_ptr<UIElement>(child));
	}
}

void UIElement::addChild(std::shared_ptr<UIElement> child)
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

UIElement* UIElement::getParent() const
{
	return parent;
}

void UIElement::setParent(UIElement* newParent)
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

void UIElement::removeFromParentsChildren()
{
	//Search parent's children for this object
	auto it = std::find_if(parent->children.begin(), parent->children.end(),
		[this](const std::shared_ptr<UIElement> obj)
		{
			return obj.get() == this;
		});
	if (it != parent->children.end())
	{
		parent->children.erase(it);
	}
	else
	{
		DEBUG_PRINTLN("Error attempting to remove child UIElement from parent: child not present in parent's list of children");
	}
}

glm::vec2 UIElement::computeEffectivePosition() const
{
	if (parent == nullptr)
	{
		return position;
	}
	return parent->computeEffectivePosition() + position;
}