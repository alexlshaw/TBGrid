#pragma once

#include <vector>
#include <cstring>
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.h"
#include "Texture.h"

void initText2D(glm::vec2 screenSize);
void printText2D(const char* text, glm::vec2 pos, float size);
void cleanupText2D();