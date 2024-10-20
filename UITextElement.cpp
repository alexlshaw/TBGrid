#include "UITextElement.h"

UITextElement::UITextElement(std::string text, glm::vec2 position, float scale, glm::vec3 color, FontAtlas* font)
	:	UIElement(position),
		scale(scale),
		color(color),
		font(font),
		text(text)
{
	if (!textShader)
	{
		DEBUG_PRINTLN("Loading text2d shader");
		textShader = GraphicsResourceManager::getInstance().loadShader("ui/text2d");
	}
}

UITextElement::~UITextElement() {}

void UITextElement::draw(Shader* shader, const glm::mat4& projection) const
{
	//Ignore the generic UI shader that was passed in, and use the text-specific shader we have
	textShader->use();
	textShader->setUniform("textColor", color);
	textShader->setUniform("projection", projection);
	textShader->setUniform("text", 0);
	glBindTexture(GL_TEXTURE_2D, font->textureID);

	glm::vec2 effectivePosition = computeEffectivePosition();
	float x = effectivePosition.x;
	float y = effectivePosition.y;
	for (auto ch : text)
	{
		if (font->characters[ch].ax == 0)
		{
			//must be a character in the skip list
			continue;
		}
		// position on screen
		float xpos = x + font->characters[ch].bl * scale;
		float ypos = y - (font->characters[ch].bh - font->characters[ch].bt) * scale;
		float w = font->characters[ch].bw * scale;
		float h = font->characters[ch].bh * scale;

		glm::mat4 matrix{ 1.0f };
		matrix = glm::translate(matrix, { xpos, ypos, 0.0 });
		matrix = glm::scale(matrix, { w,h,0.0f });
		// pick texture coordinates
		glm::vec4 vec{};
		vec.x = font->characters[ch].tx;        //x pos in textureatlas
		vec.y = font->characters[ch].ty;        //y pos in textureatlas

		vec.z = font->characters[ch].bw / (float)font->textureWidth;   //x texture coordinate
		vec.w = font->characters[ch].bh / (float)font->textureHeight;  //y texture coordinate

		textShader->setUniform("model", matrix);
		textShader->setUniform("coord", vec);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// advance cursors for next glyph     
		x += font->characters[ch].ax * scale;
	}
}
//Initialise outside of class
Shader* UITextElement::textShader = nullptr;