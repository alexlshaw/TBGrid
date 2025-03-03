#pragma once

#include "assimp/scene.h"
#include "AnimatedModel.h"
#include "glm/glm.hpp"
#include <map>
#include <string>
#include <string_view>
#include <vector>

struct AssimpNodeData
{
	glm::mat4 transformation{};
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children{};
};

class Animation
{
private:
	void readMissingBones(const aiAnimation* animation, AnimatedModel& model);
	void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
	float duration = 0.0f;
	float ticksPerSecond = 0.0f;
	std::vector<Bone> bones;
	AssimpNodeData rootNode;
	std::map<std::string, BoneInfo> boneInfoMap;
	AnimatedModel* model;
public:
	Animation() = default;
	Animation(const std::string& animationPath, AnimatedModel* model);
	~Animation() {}
	Bone* findBone(const std::string_view name);
	inline float getTicksPerSecond() const { return ticksPerSecond; }
	inline float getDuration() const { return duration; }
	inline const AssimpNodeData& getRootNode() { return rootNode; }
	inline const std::map<std::string, BoneInfo>& getBoneIDMap() { return boneInfoMap; }
	AnimatedModel* getModel();
};