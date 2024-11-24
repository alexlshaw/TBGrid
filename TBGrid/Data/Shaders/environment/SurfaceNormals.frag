#version 330

precision highp float;

in vec3 fragNormal;

out vec4 finalColor;

void main(void)
{
    finalColor = vec4(abs(fragNormal), 1.0);
}