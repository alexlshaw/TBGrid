#version 330

precision highp float;

out vec4 finalColor;

in vec2 textureCoords;

uniform sampler2D diffuseMap;

void main()
{
	vec4 texel = texture(diffuseMap, textureCoords);
	finalColor = vec4(texel.xyz, 1.0);
}