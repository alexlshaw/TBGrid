#include "Animation.h"
#include "DebuggingTools.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "Bone.h"
#include "Utilities.h"
#include <format>

Animation::Animation(const std::string& animationPath, AnimatedModel* model)
	: model(model)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
	if (scene)
	{
		//TODO: what if the scene has multiple animations? It seems like I probably want an instance of this class per animation?
		//In that case, maybe I should be passing in an index parameter to this ctor or something
		auto animation = scene->mAnimations[0];
		duration = static_cast<float>(animation->mDuration);
		ticksPerSecond = static_cast<float>(animation->mTicksPerSecond);
		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		globalTransformation = globalTransformation.Inverse();
		readHierarchyData(rootNode, scene->mRootNode);
		readMissingBones(animation, *model);
	}
	else
	{
		DEBUG_PRINTLN(std::format("Failed to load animation: {}", animationPath));
	}
}

Bone* Animation::findBone(const std::string& name)
{
	auto iter = std::find_if(bones.begin(), bones.end(), [&](const Bone& bone)
		{
			return bone.getBoneName() == name;
		});
	if (iter == bones.end())
	{
		return nullptr;
	}
	return &(*iter);
}

AnimatedModel* Animation::getModel()
{
	return model;
}

void Animation::readMissingBones(const aiAnimation* animation, AnimatedModel& model)
{
	int size = animation->mNumChannels;
	auto& boneMap = model.getBoneInfoMap();
	int& boneCount = model.getBoneCount();

	for (int i = 0; i < size; i++)
	{
		auto channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;
		//if there is no bone in our info map with the matching name
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			boneInfoMap[boneName].id = boneCount;
			boneCount++;
		}
		bones.push_back(Bone(boneName, boneInfoMap[boneName].id, channel));
	}
	boneInfoMap = boneMap;
}

void Animation::readHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
	if (src)
	{
		dest.name = src->mName.data;
		dest.transformation = AssimpGLMHelpers::convertMatrixToGLM(src->mTransformation);
		dest.childrenCount = src->mNumChildren;
		for (unsigned int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			readHierarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
	else
	{
		DEBUG_PRINTLN("Failed to read animation hierarchy data");
	}
}