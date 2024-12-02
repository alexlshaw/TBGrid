#pragma once

#include <string>
#include <vector>
#include "assimp/scene.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

//Keyframe structs
struct KeyPosition
{
	glm::vec3 position;
	float time;
};

struct KeyRotation
{
	glm::quat orientation;
	float time;
};

struct KeyScale
{
	glm::vec3 scale;
	float time;
};

struct BoneInfo
{
	int id;	//index in finalBoneMatrices
	glm::mat4 offset;	//transforms vertex from model to bone space
};

class Bone
{
private:
	std::string name;
	glm::mat4 localTransform;
	int id;
	std::vector<KeyPosition> keyPositions;
	std::vector<KeyRotation> keyRotations;
	std::vector<KeyScale> keyScales;
	float getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
	glm::mat4 interpolatePosition(float animationTime);
	glm::mat4 interpolateRotation(float animationTime);
	glm::mat4 interpolateScale(float animationTime);
public:
	Bone(const std::string& name, int ID, const aiNodeAnim* channel);
	void update(float animationTime);
	int getPositionIndex(float animationTime);
	int getRotationIndex(float animationTime);
	int getScaleIndex(float animationTime);
	std::string_view getBoneName() const;
	glm::mat4& getLocalTransform();
};