#include "StaticMesh.h"

StaticMesh::StaticMesh(Mesh* mesh, Material* material)
{
	this->mesh = mesh;
	materials.push_back(material);
	name = mesh->name;
	castsShadows = true;
}

StaticMesh::StaticMesh(std::string mesh, std::string material)
{
	GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
	this->mesh = resourceManager.loadMesh(mesh);
	name = mesh;
	materials.push_back(resourceManager.loadMaterial(material));
}

void StaticMesh::draw(int renderPass)
{
	if (renderPass >= 0)
	{
		Transform t = computeEffectiveTransform();
		materials[renderPass]->setTransform(t);
	}
	mesh->draw();
}

Material* StaticMesh::getMaterial()
{
	if (materials.size() > 0)
	{
		return materials[0];
	}
	return nullptr;
}
