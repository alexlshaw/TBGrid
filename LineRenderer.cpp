#include "LineRenderer.h"

LineRenderer::LineRenderer()
	: colour(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f))
{
	materials.push_back(GraphicsResourceManager::getInstance().loadMaterial("LineRenderer"));
}

LineRenderer::LineRenderer(glm::vec4 colour)
	: colour(colour)
{
	materials.push_back(GraphicsResourceManager::getInstance().loadMaterial("LineRenderer"));
}

void LineRenderer::draw(int renderPass)
{
	materials[renderPass]->setTransform(computeEffectiveTransform());
	if (renderPass == 0)
	{
		lineMesh.draw(GL_LINES);
	}
}

void LineRenderer::setSegments(std::vector<glm::vec3> segments)
{
	this->segments = segments;
	regenerateMesh();
}

void LineRenderer::setColour(glm::vec4 colour)
{
	this->colour = colour;
	regenerateMesh();	//Unfortunately if we change the colour we have to regenerate the mesh, since it's a property of the vertices
}

void LineRenderer::generateSegmentsFromPoints(std::vector<glm::vec3> points)
{
	segments.clear();
	int newSize = static_cast<int>(points.size()) * 2;
	segments.reserve(newSize);
	for (unsigned int i = 0; i < points.size() - 1; i++)
	{
		segments.push_back(points[i]);
		segments.push_back(points[i + 1]);
	}
	regenerateMesh();
}

void LineRenderer::regenerateMesh()
{
	std::vector<ColouredVertex> vertices;
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < segments.size(); i++)
	{
		ColouredVertex v;
		v.position = glm::vec4(segments[i].x, segments[i].y, segments[i].z, 1.0f);
		v.color = colour;
		v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
		v.texCoords = glm::vec2();
		vertices.push_back(v);
		indices.push_back(i);
	}
	lineMesh.load(vertices, indices);
}