#pragma once

#include "glm\glm.hpp"

namespace Lighting
{
	constexpr auto MAX_LIGHT_COUNT = 32;	//This has to match the value in the shader so the uniform block has a consistent size
}

//simple representation of a light
struct alignas(16) Light
{
	alignas(16) glm::vec4 ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	alignas(16) glm::vec4 diffuse = { 0.0f, 0.0f, 0.0f, 0.0f };
	alignas(16) glm::vec4 specular = { 0.0f, 0.0f, 0.0f, 0.0f };
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
	alignas(16) PointLight pointLights[Lighting::MAX_LIGHT_COUNT];
	int lightCount;	//The number of actual point lights - any past this are ignored
};