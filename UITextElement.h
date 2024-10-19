#pragma once

#include "Text2d.h"
#include <string>
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "Shader.h"
#include "Vertex.h"


class UITextElement
{
private:
	unsigned int vao, vbo;
public:
	bool enabled = true;
	glm::vec3 color;
	std::string text;
	glm::vec2 position;
	FontAtlas* font;
	float scale;
	UITextElement(std::string text, glm::vec2 position, float scale, glm::vec3 color, FontAtlas* font);
	~UITextElement();
	void draw(Shader* textShader, glm::mat4 projection) const;
};