#include "Texture.h"
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#define STB_IMAGE_IMPLEMENTATION
#pragma warning(push, 0)
#include "external/stb_image.h"
#pragma warning(pop)

Texture::Texture()
	: textureIndex(-1),
	repeats(true)
{}

Texture::Texture(const char* fileName, bool repeats)
	: repeats(repeats)
{
	load(fileName);
}

Texture::~Texture()
{
	glDeleteTextures(1, &textureIndex);
}

void Texture::load(const char* fileName)
{
	int width, height, comp = 0;
	unsigned char* img = 0;
	stbi_set_flip_vertically_on_load(1);
	img = stbi_load(fileName, &width, &height, &comp, STBI_rgb_alpha);
	glGenTextures(1, &textureIndex);
	glBindTexture(GL_TEXTURE_2D, textureIndex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	//Set texture properties
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLfloat fLargest;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fLargest);

	stbi_image_free(img);
}

void Texture::use() const
{
	glBindTexture(GL_TEXTURE_2D, textureIndex);
}

void Texture::loadFromPixels(std::vector<GLubyte> pixels, int width, int height)
{
	glGenTextures(1, &textureIndex);
	glBindTexture(GL_TEXTURE_2D, textureIndex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)&pixels[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}