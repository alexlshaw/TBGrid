#include "LightMarker.h"
#include "GraphicsResourceManager.h"

LightMarker::LightMarker(PointLight& light)
	: GameObject()
{
	GraphicsResourceManager& resourceManager = GraphicsResourceManager::getInstance();
	mesh = resourceManager.loadMesh("unit_sphere");
	Material* mat = resourceManager.loadMaterial("LightMarker");
	if (mat)
	{
		materials.push_back(mat);
	}
	transform.setScale({ 0.05f, 0.05f, 0.05f });	//Light markers are always small
	transform.setPosition(light.position);
	lightColor = computeLightColor(light);
	name = "Light Marker";
	onlyDrawInDebug = true;
}

void LightMarker::draw(int renderPass)
{
	Transform t = computeEffectiveTransform();
	materials[renderPass]->setTransform(t);
	materials[renderPass]->setProperty("albedo", lightColor);
	if (renderPass == 0)
	{
		mesh->draw();
	}
}

glm::vec4 LightMarker::computeLightColor(const PointLight& light)
{
	glm::vec4 sum = light.ambient + light.diffuse + light.specular;
	//scale the values down to the 0...1 range
	float largestTerm = glm::max(sum.r, glm::max(sum.g, sum.b));
	sum /= largestTerm;
	sum.a = 1.0f;	//a is normally 0 for the terms of a light, but will be treated as transparency for the mesh, so we need to manually set it to opaque
	return sum;
}