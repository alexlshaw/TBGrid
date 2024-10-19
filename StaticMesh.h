#pragma once

#include <string>

#include "GameObject.h"
#include "GraphicsResourceManager.h"

//A gameobject that represents a simple, unchanging mesh displayed within the scene
//Not to be confused with the Mesh class, which represents the actual vertex graph
class StaticMesh : public GameObject
{
private:
	Mesh* mesh;
public:
	StaticMesh(Mesh* mesh, Material* material);
	StaticMesh(std::string mesh, std::string material);
	void draw(int renderPass);
};