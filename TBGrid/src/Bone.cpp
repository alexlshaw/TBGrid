#include "Bone.h"
#include "Utilities.h"

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
	: name(name),
	id(ID),
	localTransform(1.0f)
{
	//Load the keyframe data
	keyPositions.reserve(channel->mNumPositionKeys);
	keyRotations.reserve(channel->mNumRotationKeys);
	keyScales.reserve(channel->mNumScalingKeys);
	for (unsigned int i = 0; i < channel->mNumPositionKeys; i++)
	{
		aiVector3D aiPosition = channel->mPositionKeys[i].mValue;
		float time = static_cast<float>(channel->mPositionKeys[i].mTime);
		KeyPosition data{ AssimpGLMHelpers::convertVec3ToGLM(aiPosition), time };
		keyPositions.push_back(data);
	}
	for (unsigned int i = 0; i < channel->mNumRotationKeys; i++)
	{
		aiQuaternion aiRotation = channel->mRotationKeys[i].mValue;
		float time = static_cast<float>(channel->mRotationKeys[i].mTime);
		KeyRotation data{ AssimpGLMHelpers::convertQuatToGLM(aiRotation), time };
		keyRotations.push_back(data);
	}
	for (unsigned int i = 0; i < channel->mNumScalingKeys; i++)
	{
		aiVector3D aiScale = channel->mScalingKeys[i].mValue;
		float time = static_cast<float>(channel->mScalingKeys[i].mTime);
		KeyScale data{ AssimpGLMHelpers::convertVec3ToGLM(aiScale), time };
		keyScales.push_back(data);
	}
}

void Bone::update(float animationTime)
{
	glm::mat4 translation = interpolatePosition(animationTime);
	glm::mat4 rotation = interpolateRotation(animationTime);
	glm::mat4 scale = interpolateScale(animationTime);
	localTransform = translation * rotation * scale;
}

int Bone::getPositionIndex(float animationTime)
{
	for (size_t i = 0; i < keyPositions.size() - 1; i++)
	{
		if (animationTime < keyPositions[i + 1].time)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

int Bone::getRotationIndex(float animationTime)
{
	for (size_t i = 0; i < keyRotations.size() - 1; i++)
	{
		if (animationTime < keyRotations[i + 1].time)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

int Bone::getScaleIndex(float animationTime)
{
	for (size_t i = 0; i < keyScales.size() - 1; i++)
	{
		if (animationTime < keyScales[i + 1].time)
		{
			return static_cast<int>(i);
		}
	}
	return -1;
}

std::string_view Bone::getBoneName() const
{
	return name;
}

glm::mat4& Bone::getLocalTransform()
{
	return localTransform;
}

float Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	//The proportion of the way we are between the two timestamps (calculate t for interpolation)
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	return midWayLength / framesDiff;
}

glm::mat4 Bone::interpolatePosition(float animationTime)
{
	if (keyPositions.size() == 1)
	{
		return glm::translate(glm::mat4(1.0f), keyPositions[0].position);
	}
	int index0 = getPositionIndex(animationTime);
	int index1 = index0 + 1;
	float scaleFactor = getScaleFactor(keyPositions[index0].time, keyPositions[index1].time, animationTime);
	glm::vec3 finalPosition = glm::mix(keyPositions[index0].position, keyPositions[index1].position, scaleFactor);
	return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::interpolateRotation(float animationTime)
{
	if (keyRotations.size() == 1)
	{
		return glm::mat4_cast(glm::normalize(keyRotations[0].orientation));
	}
	int index0 = getRotationIndex(animationTime);
	int index1 = index0 + 1;
	float scaleFactor = getScaleFactor(keyRotations[index0].time, keyRotations[index1].time, animationTime);
	glm::quat finalRotation = glm::slerp(keyRotations[index0].orientation, keyRotations[index1].orientation, scaleFactor);
	return glm::mat4_cast(finalRotation);
}

glm::mat4 Bone::interpolateScale(float animationTime)
{
	if (keyScales.size() == 1)
	{
		return glm::scale(glm::mat4(1.0f), keyScales[0].scale);
	}
	int index0 = getScaleIndex(animationTime);
	int index1 = index0 + 1;
	float scaleFactor = getScaleFactor(keyScales[index0].time, keyScales[index1].time, animationTime);
	glm::vec3 finalScale = glm::mix(keyScales[index0].scale, keyScales[index1].scale, animationTime);
	return glm::scale(glm::mat4(1.0f), finalScale);
}