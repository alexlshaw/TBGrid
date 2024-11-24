#version 330

//Shader to generate a depth map

precision highp float;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;

uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;

out vec2 coords;

void main(void)
{
	coords = texCoords;
	gl_Position = projectionViewMatrix * modelMatrix * vec4(position.xyz, 1.0);
}