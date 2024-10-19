#include "UITextElement.h"

UITextElement::UITextElement(std::string text, glm::vec2 position, float scale, glm::vec3 color, FontAtlas* font)
	:	position(position),
		scale(scale),
		color(color),
		font(font),
		text(text)
{
	//create a basic quad for displaying the text
	float xpos = 0.0;
	float ypos = 0.0;
	float l = 1.0f;
	std::vector<UIVertex> vertices = {
		UIVertex{glm::vec2(xpos,ypos + l), glm::vec2(0.0f, 0.0f)},
		UIVertex{glm::vec2(xpos,ypos), glm::vec2(0.0f,1.0f)},
		UIVertex{glm::vec2(xpos + l,ypos), glm::vec2(1.0f, 1.0f)},
		UIVertex{glm::vec2(xpos,ypos + l), glm::vec2(0.0f, 0.0f)},
		UIVertex{glm::vec2(xpos + l,ypos), glm::vec2(1.0f,1.0f)},
		UIVertex{glm::vec2(xpos + l,ypos + l), glm::vec2(1.0f, 0.0f)}
	};

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertex) * 6, &vertices[0], GL_DYNAMIC_DRAW);
	setUIVertexAttribs();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

UITextElement::~UITextElement() {}

void UITextElement::draw(Shader* textShader, glm::mat4 projection) const
{
	textShader->use();
	textShader->setUniform("textColor", color);
	textShader->setUniform("projection", projection);
	textShader->setUniform("text", 18);
	glActiveTexture(GL_TEXTURE18);
	glBindTexture(GL_TEXTURE_2D, font->textureID);
	glBindVertexArray(vao);

	float x = position.x;
	float y = position.y;
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