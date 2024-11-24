#version 330

//Debug shader to display a quad in screen space showing the contents of a depth map

precision highp float;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;

out vec2 textureCoords;

void main(void)
{
	textureCoords = texCoords;
	gl_Position = vec4(position.xyz, 1.0);
}