#version 330

precision highp float;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in ivec4 boneIds;	//xyzw correspond to array entries
layout (location = 4) in vec4 weights; //xyzw correspond to array entries

uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;

const int MAX_BONES = 100; //Has a matching value in Graphics/Animation/Animator.h, make sure they're equal
const int MAX_BONE_INFLUENCE = 4;	//Has a matching value in Graphics/Vertex.h, make sure they're equal
uniform mat4 finalBoneMatrices[MAX_BONES];

out vec2 textureCoords;

void main()
{
	vec4 totalPosition = vec4(0.0f);
	for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
	{
		if (boneIds[i] == -1)
		{
			continue;
		}
		if (boneIds[i] >= MAX_BONES)
		{
			totalPosition = vec4(position, 1.0f);
			break;
		}
		vec4 localPosition = finalBoneMatrices[boneIds[i]] * vec4(position, 1.0f);
		totalPosition += localPosition * weights[i];
		vec3 localNormal = mat3(finalBoneMatrices[boneIds[i]]) * normal; //TODO: hmmm
	}
	gl_Position = projectionViewMatrix * modelMatrix * totalPosition;
}