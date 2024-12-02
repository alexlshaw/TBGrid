#version 330

precision highp float;

out vec4 finalColor;

in vec2 textureCoords;

uniform sampler2D textureDiffuse;

void main()
{
	finalColor = texture(textureDiffuse, textureCoords);
}