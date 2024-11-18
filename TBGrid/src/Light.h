#pragma once

#include "glm\glm.hpp"

constexpr auto NR_POINT_LIGHTS = 4;

//simple representation of a light
struct Light
{
	glm::vec3 ambient = { 0.0f, 0.0f, 0.0f };
	glm::vec3 diffuse = { 0.0f, 0.0f, 0.0f };
	glm::vec3 specular = { 0.0f, 0.0f, 0.0f };
};

struct DirectionalLight : public Light
{
	glm::vec3 direction = { 0.0f, 0.0f, 1.0f };
};

struct PointLight : public Light
{
	float constantAttenuation = 0.0f;
	float linearAttenuation = 0.0f;
	float quadraticAttenuation = 0.0f;
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
};

//Maps to light uniform in shaders
struct LightBlock
{
	DirectionalLight dirLight;
	PointLight pointLights[NR_POINT_LIGHTS];
};