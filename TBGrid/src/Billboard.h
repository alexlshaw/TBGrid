#pragma once

#include "GameObject.h"
#include "Mesh.h"
#include "Texture.h"
#include <string>

class Billboard : public GameObject
{
private:
	Mesh* mesh;
public:
	Billboard(std::string textureName);
	void draw(int renderPass) override;
	Texture* tex;
};