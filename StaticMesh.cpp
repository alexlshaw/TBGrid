#include "StaticMesh.h"

StaticMesh::StaticMesh(Mesh* mesh, Material* material)
{
	this->mesh = *mesh;
	materials.push_back(material);
	name = mesh->name;
}

StaticMesh::StaticMesh(std::string mesh, std::string material, GraphicsResourceManager* resourceManager)
{
	this->mesh = *resourceManager->loadMesh(mesh);
	name = mesh;
	materials.push_back(resourceManager->loadMaterial(material));
}

void StaticMesh::draw(int renderPass, Transform transform)
{
	materials[renderPass]->setTransform(transform);
	if (renderPass == 0)
	{
		mesh.draw();
	}
}