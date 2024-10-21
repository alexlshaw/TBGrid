#pragma once

#include <array>
#include <format>
#include <string_view>
#include "DebuggingTools.h"
#include "glad/gl.h"
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

class FontAtlas
{
public:
    unsigned int textureID{};
    unsigned int textureWidth{};
    unsigned int textureHeight{};
    std::array<Character, 128> characters{};
    FontAtlas(std::string_view fontName);
    ~FontAtlas();
    int glpyhSize = 48;
};