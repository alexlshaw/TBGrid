#version 330

precision highp float;

in Fragment
{
	vec2 texCoords;
	vec3 normal;
	vec4 color;
} fragment;

out vec4 finalColor;

uniform sampler2D tex;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 lightPosition;
uniform vec4 albedo;

void main(void)
{
	vec3 light = ambient + diffuse * max(dot(fragment.normal, lightPosition), 0.0);

	vec4 texel = texture(tex, fragment.texCoords) * fragment.color * albedo;
	if (texel.a < 0.01)
	{
		discard;
	}
	finalColor = vec4(light * texel.xyz, texel.w);
}