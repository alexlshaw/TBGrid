#pragma once

#include <string>
#include "DebuggingTools.h"
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "GraphicsResourceManager.h"
#include "Shader.h"
#include "Text2d.h"
#include "UIElement.h"
#include "Vertex.h"


class UITextElement : public UIElement
{
private:
	static Shader* textShader;
public:
	glm::vec3 color;
	std::string text;
	FontAtlas* font;
	float scale;
	UITextElement(std::string text, glm::vec2 position, float scale, glm::vec3 color, FontAtlas* font);
	~UITextElement();
	void draw(Shader* shader, const glm::mat4& projection) const override;
};