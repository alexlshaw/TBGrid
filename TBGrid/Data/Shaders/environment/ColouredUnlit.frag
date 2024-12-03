#version 330

precision highp float;

in Fragment
{
	vec2 texCoords;
	vec4 color;
} fragment;

out vec4 finalColor;

uniform sampler2D diffuseMap;
uniform vec4 diffuseColour;

void main(void)
{

	vec4 texel = texture(diffuseMap, fragment.texCoords) * fragment.color * diffuseColour;
	if (texel.a < 0.01)
	{
		discard;
	}
	finalColor = texel;
}