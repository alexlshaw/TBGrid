#version 330

precision highp float;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;

uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

out Fragment
{
	vec3 position;
	vec2 texCoords;
	vec3 normal;
	vec4 color;
	vec4 positionLightSpace;
} fragment;

void main(void)
{
	fragment.position = vec3(modelMatrix * vec4(position.xyz, 1.0));
	fragment.texCoords = texCoords;
	fragment.color = color;
	fragment.normal = normalize(normalMatrix * normal);
	fragment.positionLightSpace = lightSpaceMatrix * vec4(fragment.position, 1.0);
	gl_Position = projectionViewMatrix * modelMatrix * vec4(position.xyz, 1.0);
}