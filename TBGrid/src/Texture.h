#pragma once

#include <cstdlib>
#include <vector>
#include "glad/gl.h"

class Texture
{
private:
    GLuint textureIndex;
    bool repeats;
public:
    Texture();
    Texture(const char* filename, bool repeats = true);
    ~Texture();
    void load(const char* fileName);
    void loadFromPixels(std::vector<GLubyte> pixels, int width, int height);	//4 elements in array define a pixel in rgba format
    void use() const;
};