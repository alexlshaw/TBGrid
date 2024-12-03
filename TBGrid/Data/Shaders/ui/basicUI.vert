#version 330 core
layout (location = 0) in vec2 vertex; 
layout (location = 1) in vec2 coords; 

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    vec4 finalPos =  projection * model * vec4(vertex.xy, 0.0, 1.0);
    gl_Position = vec4(finalPos.xy, -1.0, 1.0); //force UI to be drawn at depth -1 in NDC
    TexCoords = coords;
}