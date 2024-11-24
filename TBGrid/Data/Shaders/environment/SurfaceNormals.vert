#version 330

precision highp float;

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;

uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

out vec3 fragNormal;

void main(void)
{
	fragNormal = normalize(normalMatrix * normal);
	vec4 newPos = projectionViewMatrix * modelMatrix * position;
	gl_Position = newPos;
}