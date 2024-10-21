#include "FontAtlas.h"

FontAtlas::FontAtlas(std::string_view fontName)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        DEBUG_PRINTLN(" Failed to init FreeType Library");
    }
    FT_Face face;
    if (FT_New_Face(ft, fontName.data(), 0, &face))
    {
        DEBUG_PRINTLN(std::format(" Failed to load font: {}", fontName));
    }
    FT_Set_Pixel_Sizes(face, 0, glpyhSize);
    FT_GlyphSlot g = face->glyph;

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

		if (row_width + g->bitmap.width + 1 >= 1024)
		{
			textureWidth = std::max(textureWidth, row_width);
			textureHeight += row_height;
			row_width = 0;
			row_height = 0;
		}
		row_width += g->bitmap.width + 1;
		row_height = std::max(row_height, g->bitmap.rows);
	}

	textureWidth = std::max(textureWidth, row_width);
	textureHeight += row_height;
	glActiveTexture(GL_TEXTURE18);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, textureWidth, textureHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
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

		if (xOffset + g->bitmap.width + 1 >= 1024)
		{
			yOffset += row_height;
			row_height = 0;
			xOffset = 0;
		}

		glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);

		characters[i].ax = g->advance.x >> 6;
		characters[i].ay = g->advance.y >> 6;

		characters[i].bw = g->bitmap.width;
		characters[i].bh = g->bitmap.rows;

		characters[i].bl = g->bitmap_left;
		characters[i].bt = g->bitmap_top;

		characters[i].tx = xOffset / (float)textureWidth;
		characters[i].ty = yOffset / (float)textureHeight;

		row_height = std::max(row_height, g->bitmap.rows);
		xOffset += g->bitmap.width + 1;
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	DEBUG_PRINT_GL_ERRORS("FontAtlas::FontAtlas()");
}

FontAtlas::~FontAtlas()
{
	if (textureID)
	{
		glDeleteTextures(1, &textureID);
	}
}