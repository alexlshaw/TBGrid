#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.h"
#include "Texture.h"
#include "UIElement.h"

class UIImageElement : public UIElement
{
private:
	Texture* image;
	glm::vec2 size;
public:
	UIImageElement(glm::vec2 position, glm::vec2 size, Texture* image);
	~UIImageElement() {}
	void draw(Shader* shader, const glm::mat4& projection) const override;
};