#include "Animator.h"

Animator::Animator(Animation* animation)
	: currentAnimation(animation)
{
	//init our bone matrices with identity matrix
	finalBoneMatrices = std::vector<glm::mat4>(AnimationConstants::MAX_BONES, glm::mat4(1.0f));
}

void Animator::updateAnimation(float delta)
{
	deltaTime = delta;
	if (currentAnimation)
	{
		currentTime += currentAnimation->getTicksPerSecond() * delta;
		currentTime = fmod(currentTime, currentAnimation->getDuration());
		//TODO: I suspect that here I will need to pass in the parent transform (if any) of the model
		calculateBoneTransform(&currentAnimation->getRootNode(), glm::mat4(1.0f));
	}
}

void Animator::playAnimation(Animation* animation)
{
	currentAnimation = animation;
	currentTime = 0.0f;
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* bone = currentAnimation->findBone(nodeName);
	if (bone)
	{
		bone->update(currentTime);
		nodeTransform = bone->getLocalTransform();
	}
	glm::mat4 globalTransform = parentTransform * nodeTransform;
	auto& boneInfoMap = currentAnimation->getBoneIDMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		//can't use [] because I'm dealing with a const &
		int idx = boneInfoMap.at(nodeName).id;
		glm::mat4 offset = boneInfoMap.at(nodeName).offset;
		finalBoneMatrices[idx] = globalTransform * offset;
	}
	for (int i = 0; i < node->childrenCount; i++)
	{
		calculateBoneTransform(&node->children[i], globalTransform);
	}
}

std::vector<glm::mat4>& Animator::getFinalBoneMatrices()
{
	return finalBoneMatrices;
}

AnimatedModel* Animator::getModel()
{
	if (currentAnimation)
	{
		return currentAnimation->getModel();
	}
	return nullptr;
}
