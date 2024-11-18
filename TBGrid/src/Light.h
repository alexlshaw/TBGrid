#pragma once

#include "glm\glm.hpp"

constexpr auto NR_POINT_LIGHTS = 4;

//simple representation of a light
struct alignas(16) Light
{
	alignas(16) glm::vec4 ambient = { 0.0f, 0.0f, 0.0f, 1.0f };
	alignas(16) glm::vec4 diffuse = { 0.0f, 0.0f, 0.0f, 1.0f };
	alignas(16) glm::vec4 specular = { 0.0f, 0.0f, 0.0f, 1.0f };
};

struct alignas(16) DirectionalLight : public Light
{
	alignas(16) glm::vec4 direction = { 0.0f, 0.0f, 1.0f, 0.0f };
};

struct alignas(16) PointLight : public Light
{
	alignas(16) glm::vec4 attenuation = { 0.0f, 0.0f, 0.0f, 0.0f };	//constant, linear, quadratic, ignored
	alignas(16) glm::vec4 position = { 0.0f, 0.0f, 0.0f, 0.0f };
};

//Maps to light uniform in shaders
struct alignas(16) LightBlock
{
	alignas(16) DirectionalLight dirLight;
	alignas(16) PointLight pointLights[NR_POINT_LIGHTS];
};