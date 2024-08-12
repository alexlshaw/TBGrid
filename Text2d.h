#pragma once

#include <vector>
#include <cstring>
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Texture.h"

void initText2D(int screenWidth, int screenHeight);
void printText2D(const char* text, int x, int y, int size);
void cleanupText2D();