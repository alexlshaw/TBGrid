#pragma once

#include <string>
#include "DebuggingTools.h"
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "GraphicsResourceManager.h"
#include "Shader.h"
#include "UIElement.h"
#include "Vertex.h"


class UITextElement : public UIElement
{
private:
	static Shader* textShader;
	void checkShader();
public:
	glm::vec3 color;
	std::string text;
	FontAtlas* font;
	UITextElement(std::string text, glm::vec2 position, glm::vec2 scale, glm::vec3 color);
	UITextElement(std::string text, glm::vec2 position, float scale, glm::vec3 color);		//Extra ctor since we almost always want same scale for x and y
	UITextElement(std::string text, glm::vec2 position, glm::vec2 scale, glm::vec3 color, FontAtlas* font);
	UITextElement(std::string text, glm::vec2 position, float scale, glm::vec3 color, FontAtlas* font);	//Extra ctor since we almost always want same scale for x and y
	~UITextElement();
	void draw(Shader* shader, const glm::mat4& projection) const override;
};