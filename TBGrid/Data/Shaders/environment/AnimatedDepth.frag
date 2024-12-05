#version 330

//Shader to render an animated object into the depth map

precision highp float;

uniform sampler2D diffuseMap;

in vec2 coords;

void main()
{
	//don't write to the depth buffer if the texture is transparent
	vec4 texel = texture(diffuseMap, coords);
	if (texel.a < 0.05)
	{
		discard;
	}
	//otherwise, no action required, will automatically write to the depth buffer
	//Empty shader automatically sets depth buffer values
}