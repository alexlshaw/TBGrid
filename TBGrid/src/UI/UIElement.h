#pragma once

#include <memory>
#include <vector>
#include "DebuggingTools.h"
#include "glm/glm.hpp"
#include "Shader.h"

class UIElement
{
private:
	UIElement* parent;	//Ownership: a child element has no responsibility or ownership of its parent, so we just use a raw pointer
	void removeFromParentsChildren();
protected:
	virtual void draw(Shader* shader, const glm::mat4& projection) const;
	virtual void update(float delta);
public:
	UIElement(glm::vec2 position, glm::vec2 size);
	UIElement(glm::vec2 position, glm::vec2 size, glm::vec2 scale);
	~UIElement() {}
	//Recursive draw and update functions to handle child elements
	void drawAll(Shader* shader, const glm::mat4& projection) const;
	void updateAll(float delta);
	bool enabled = true;
	glm::vec2 position;
	glm::vec2 size;	//Size of the element in pixels when drawn at scale = 1
	glm::vec2 scale;	//Multiplier for element size. Inherits from parents, so drawing a child element at scale 0.5 inside a parent element also sclaed at 0.5 will multiply size by 0.25
	std::vector<std::shared_ptr<UIElement>> children; //Ownership: A parent element has shared responsibility (with the canvas/manager) of any of its child elements
	void addChild(UIElement* child);
	void addChild(std::shared_ptr<UIElement> child);
	UIElement* getParent() const;
	void setParent(UIElement* newParent);
	virtual glm::vec2 computeEffectivePosition() const;
	virtual glm::vec2 computeEffectiveScale() const;
	virtual bool consumeClick(glm::vec2 clickLocation);
	bool contains(const glm::vec2 location) const;
};