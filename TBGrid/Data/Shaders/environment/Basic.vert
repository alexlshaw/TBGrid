#version 330

precision highp float;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;

uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;

out Fragment
{
	vec2 texCoords;
	vec4 color;
} fragment;

void main(void)
{
	fragment.texCoords = texCoords;
	fragment.color = color;
	vec4 newPos = modelMatrix * position;
	gl_Position = projectionViewMatrix * newPos;
}