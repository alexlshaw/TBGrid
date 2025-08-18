#version 330

precision highp float;

// Mesh should always be unit quad in object space, e.g. (-0.5,-0.5), (0.5,-0.5), (0.5,0.5), (-0.5,0.5)
layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoords;

uniform mat4 projectionViewMatrix;  // View-projection matrix
uniform mat4 modelMatrix;           // Full model transform (from parents)
uniform vec3 cameraRight;           // Extracted from view matrix cpu side
uniform vec3 cameraUp;              // Extracted from view matrix cpu side

out vec2 uv;

void main()
{
    //Center of billboard is final translation value from model matrix
    vec3 billboardCenter = vec3(modelMatrix[3]); // translation (world position)

    //One possibility would be to make scale take into consideration camera position, so that if a parent object is stretched on Z,
    //the billboard will stretch in that direction if the camera is looking down the X axis
    //For now however, we just do the billboard scale based on X, and assume that parents will have uniform horizontal scaling
    float scaleX = length(vec3(modelMatrix[0])); // X axis length
    float scaleY = length(vec3(modelMatrix[1])); // Y axis length

    // --- Build world position for this vertex of the quad ---
    vec3 worldPos = billboardCenter
                  + cameraRight * position.x * scaleX
                  + cameraUp    * position.y * scaleY;

    gl_Position = projectionViewMatrix * vec4(worldPos, 1.0);
    uv = texCoords;
}