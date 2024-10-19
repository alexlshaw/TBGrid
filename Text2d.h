#pragma once

#include <array>
#include <format>
#include <string>
#include <string_view>
#include <vector>
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "DebuggingTools.h"
#include "Shader.h"
#include "Texture.h"
#include "Vertex.h"

#include <ft2build.h>
#include FT_FREETYPE_H

struct Character 
{
    FT_Int ax;	// advance.x
    FT_Int ay;	// advance.y
    FT_Int bw;	// bitmap.width;
    FT_Int bh;	// bitmap.height;
    FT_Int bl;	// bitmap_left;
    FT_Int bt;	// bitmap_top;
    float tx;	// x offset of glyph in texture coordinates
    float ty;	// y offset of glyph in texture coordinates
};

struct FontAtlas
{
	unsigned int textureID{};
	unsigned int textureWidth{};
	unsigned int textureHeight{};
    std::array<Character, 128> characters{};
};

int initText2D();
FontAtlas createFontTextureAtlas(std::string_view fontName);
//Default text rendering. Kept here for useful debugging, but in practice should pretty much always use a UITextElement or similar
void RenderText(Shader* s, glm::mat4 projection, std::string_view text, float x, float y, float scale, glm::vec3 color);
void cleanupText2D();