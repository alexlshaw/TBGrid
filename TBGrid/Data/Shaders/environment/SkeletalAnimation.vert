#version 330

precision highp float;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in ivec4 boneIds;	//xyzw correspond to array entries
layout (location = 4) in vec4 weights; //xyzw correspond to array entries

const int MAX_BONES = 100; //Has a matching value in Graphics/Animation/Animator.h, make sure they're equal
const int MAX_BONE_INFLUENCE = 4;	//Has a matching value in Graphics/Vertex.h, make sure they're equal
uniform mat4 finalBoneMatrices[MAX_BONES];
uniform mat4 projectionViewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;
uniform mat4 lightSpaceMatrix;

out Fragment
{
	vec3 position;
	vec2 texCoords;
	vec3 normal;
	vec4 positionLightSpace;
} fragment;

void main()
{
	vec4 totalPosition = vec4(0.0f);
	vec3 totalNormal = vec3(0.0f);
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
		vec3 localNormal = mat3(finalBoneMatrices[boneIds[i]]) * normal;
		totalNormal += localNormal * weights[i];
	}
	fragment.position = vec3(modelMatrix * vec4(totalPosition.xyz, 1.0));
	fragment.texCoords = texCoords;
	fragment.normal = normalize(normalMatrix * totalNormal);
	fragment.positionLightSpace = lightSpaceMatrix * vec4(fragment.position, 1.0);
	gl_Position = projectionViewMatrix * modelMatrix * totalPosition;
}