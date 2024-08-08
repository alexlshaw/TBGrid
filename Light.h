#pragma once

#include "glm\glm.hpp"

//simple representation of a point light (for a directional light, assume direction vector will be -position)
class Light
{
private:
public:
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	glm::vec3 position;
	Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, glm::vec3 position) 
		: ambient(ambient), diffuse(diffuse), specular(specular), position(position) {}
};