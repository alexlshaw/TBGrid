#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform vec4 coord;

void main()
{   
	vec2 v = TexCoords*coord.zw + coord.xy;
	
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, v).r);
    color = vec4(textColor, 1.0) * sampled;
}