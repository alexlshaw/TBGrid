#version 330

precision highp float;

out vec4 finalColor;

in vec2 textureCoords;

uniform sampler2D textureDiffuse;

void main()
{
	vec4 texel = texture(textureDiffuse, textureCoords);
	finalColor = vec4(texel.xyz, 1.0);
}