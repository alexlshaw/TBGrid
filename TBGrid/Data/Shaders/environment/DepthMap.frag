#version 330

//Debug shader to display a quad in screen space showing the contents of a depth map

precision highp float;

in vec2 textureCoords;

out vec4 finalColor;

uniform sampler2D diffuseMap;

void main(void)
{
	float texel = texture(diffuseMap, textureCoords).r;
	finalColor = vec4(vec3(texel), 1.0);
}