#version 330

precision highp float;

in Fragment
{
	vec2 texCoords;
	vec4 color;
} fragment;

out vec4 finalColor;

uniform sampler2D tex;
uniform vec4 albedo;

void main(void)
{

	vec4 texel = texture(tex, fragment.texCoords) * fragment.color * albedo;
	if (texel.a < 0.01)
	{
		discard;
	}
	finalColor = texel;
}