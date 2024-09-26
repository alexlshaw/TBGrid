#pragma once

#include <fstream>
#include <vector>
#include <string>

#include "DebuggingTools.h"
#include "glad/gl.h"
#include "Vertex.h"

class Mesh
{
private:
	GLuint vbo, vao, ibo;
	int indexCount;
	void countObj(std::ifstream& input, unsigned int& vertexPositionCount, unsigned int& vertexUVCount, unsigned int& vertexNormalCount, unsigned int& indexCount);
public:
	Mesh();
	Mesh(std::string name, std::vector<SimpleVertex> vertices, std::vector<unsigned int> indices);
	Mesh(std::string name, std::vector<ColouredVertex> vertices, std::vector<unsigned int> indices);
	std::string name;
	void LoadFromObj(std::string fileName);
	~Mesh();
	void draw() const;
};