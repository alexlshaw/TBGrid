#include "Billboard.h"
#include "GraphicsResourceManager.h"
#include "Transform.h"

Billboard::Billboard(std::string textureName)
{
	tex = GraphicsResourceManager::getInstance().loadTexture(textureName);
	mesh = GraphicsResourceManager::getInstance().loadMesh("billboard");
}

void Billboard::draw(int renderPass)
{
	if (renderPass == 0)
	{
		tex->use();
		mesh->draw();
	}
}
