#version 330

precision highp float;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;

uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

out Fragment
{
	vec3 position;
	vec2 texCoords;
	vec3 normal;
	vec4 color;
} fragment;

void main(void)
{
	fragment.texCoords = texCoords;
	fragment.color = color;
	fragment.normal = normalize(normalMatrix * normal);
	vec4 newPos = modelMatrix * position;
	fragment.position = newPos.xyz;
	gl_Position = projectionViewMatrix * newPos;
}