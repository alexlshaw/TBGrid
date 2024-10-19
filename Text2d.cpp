#include "Text2D.h"

//ttf default text rendering stuff.
unsigned int ttfVAO, ttfVBO;
FontAtlas fontAtlas;

int initText2D()
{
	DEBUG_PRINT("Initialising Text2D...");
	
	fontAtlas = createFontTextureAtlas("Data/Fonts/Poly-Regular.otf");

	//create the vertex structure for drawing the text
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

	glGenVertexArrays(1, &ttfVAO);
	glGenBuffers(1, &ttfVBO);
	glBindVertexArray(ttfVAO);
	glBindBuffer(GL_ARRAY_BUFFER, ttfVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertex) * 6, &vertices[0], GL_DYNAMIC_DRAW);
	setUIVertexAttribs();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	DEBUG_PRINTLN(" fininshed");
	DEBUG_PRINT_GL_ERRORS("Text2d.cpp: initText2D_FT()");
	return 1;
}

FontAtlas createFontTextureAtlas(std::string_view fontName)
{
	FontAtlas atlas;
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		DEBUG_PRINTLN(" Failed to init FreeType Library");
		return atlas;
	}

	FT_Face face;
	if (FT_New_Face(ft, fontName.data(), 0, &face))
	{
		DEBUG_PRINTLN(std::format(" Failed to load font: {}", fontName));
		return atlas;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);
	FT_GlyphSlot g = face->glyph;

	auto skipCharacter = [](int c)->bool
		{
			const static std::string s = "'|@~/><^¨{}[]\\\"";
			auto t = s.find(c);
			return t != std::string::npos;
		};

	unsigned int row_width = 0;
	unsigned int row_height = 0;

	

	//Find minimum size for a texture holding all desired ASCII characters
	for (int i = 32; i < 128; i++)
	{
		if (FT_Load_Char(face, i, FT_LOAD_RENDER))
		{
			DEBUG_PRINTLN("Font Atlas: failed to load glyph:" + i);
			continue;
		}
		//ignore some chars to make the atlas smaller
		if (skipCharacter(i))
		{
			continue;
		}

		if (row_width + g->bitmap.width + 1 >= 1024)
		{
			atlas.textureWidth = std::max(atlas.textureWidth, row_width);
			atlas.textureHeight += row_height;
			row_width = 0;
			row_height = 0;
		}
		row_width += g->bitmap.width + 1;
		row_height = std::max(row_height, g->bitmap.rows);
	}
	atlas.textureWidth = std::max(atlas.textureWidth, row_width);
	atlas.textureHeight += row_height;
	glActiveTexture(GL_TEXTURE18);
	glGenTextures(1, &atlas.textureID);
	glBindTexture(GL_TEXTURE_2D, atlas.textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas.textureWidth, atlas.textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	row_height = 0;
	int xOffset = 0;
	int yOffset = 0;

	for (int i = 32; i < 128; i++)
	{
		if (FT_Load_Char(face, i, FT_LOAD_RENDER)) 
		{
			DEBUG_PRINTLN("Font Atlas: failed to load glyph:" + i);
			continue;
		}

		//ignore some chars to make the atlas smaller
		if (skipCharacter(i))
		{
			continue;
		}


		if (xOffset + g->bitmap.width + 1 >= 1024) 
		{
			yOffset += row_height;
			row_height = 0;
			xOffset = 0;
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		atlas.characters[i].ax = g->advance.x >> 6;
		atlas.characters[i].ay = g->advance.y >> 6;

		atlas.characters[i].bw = g->bitmap.width;
		atlas.characters[i].bh = g->bitmap.rows;

		atlas.characters[i].bl = g->bitmap_left;
		atlas.characters[i].bt = g->bitmap_top;

		atlas.characters[i].tx = xOffset / (float)atlas.textureWidth;
		atlas.characters[i].ty = yOffset / (float)atlas.textureHeight;

		row_height = std::max(row_height, g->bitmap.rows);
		xOffset += g->bitmap.width + 1;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	DEBUG_PRINT_GL_ERRORS("Text2d.cpp: createFontTextureAtlas()");
	return atlas;
}

void RenderText(Shader* s, glm::mat4 projection, std::string_view text, float x, float y, float scale, glm::vec3 color)
{
	s->use();
	s->setUniform("textColor", color);
	s->setUniform("projection", projection);
	s->setUniform("text", 18);
	glActiveTexture(GL_TEXTURE18);
	glBindTexture(GL_TEXTURE_2D, fontAtlas.textureID);
	glEnable(GL_BLEND);
	glBindVertexArray(ttfVAO);

	for (auto ch : text)
	{
		if (fontAtlas.characters[ch].ax == 0)
		{
			//must be a character in the skip list
			continue;
		}
		// position on screen
		float xpos = x + fontAtlas.characters[ch].bl * scale;
		float ypos = y - (fontAtlas.characters[ch].bh - fontAtlas.characters[ch].bt) * scale;
		float w = fontAtlas.characters[ch].bw * scale;
		float h = fontAtlas.characters[ch].bh * scale;

		glm::mat4 matrix{ 1.0f };
		matrix = glm::translate(matrix, { xpos, ypos, 0.0 });
		matrix = glm::scale(matrix, { w,h,0.0f });
		// pick texture coordinates
		glm::vec4 vec{};
		vec.x = fontAtlas.characters[ch].tx;        //x pos in textureatlas
		vec.y = fontAtlas.characters[ch].ty;        //y pos in textureatlas

		vec.z = fontAtlas.characters[ch].bw / (float)fontAtlas.textureWidth;   //x texture coordinate
		vec.w = fontAtlas.characters[ch].bh / (float)fontAtlas.textureHeight;  //y texture coordinate

		s->setUniform("model", matrix);
		s->setUniform("coord", vec);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// advance cursors for next glyph     
		x += fontAtlas.characters[ch].ax * scale;
	}

	glBindVertexArray(0);
	DEBUG_PRINT_GL_ERRORS("Text2d.cpp: RenderText()");
}

void cleanupText2D() 
{
	// Delete buffers
	glDeleteBuffers(1, &ttfVBO);
	glDeleteVertexArrays(1, &ttfVAO);

	// Delete texture
	glDeleteTextures(1, &(fontAtlas.textureID));
}
