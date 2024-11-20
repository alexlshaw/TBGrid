#version 330

//Shader to generate a depth map

precision highp float;

layout (location = 0) in vec4 position;

uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;

void main(void)
{
	gl_Position = projectionViewMatrix * modelMatrix * vec4(position.xyz, 1.0);
}