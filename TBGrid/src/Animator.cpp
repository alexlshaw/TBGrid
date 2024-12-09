#include "Animator.h"

Animator::Animator(AnimationGraphNode* startingState, std::vector<std::shared_ptr<AnimationGraphNode>> stateGraph)
	: allStates(stateGraph)
{
	setState(startingState);
	//init our bone matrices with identity matrix
	finalBoneMatrices = std::vector<glm::mat4>(AnimationConstants::MAX_BONES, glm::mat4(1.0f));
}

void Animator::updateAnimation(float delta)
{
	checkForTransitions();
	if (currentState->animation)
	{
		currentTime += currentState->animation->getTicksPerSecond() * delta;
		if (currentTime >= currentState->animation->getDuration())
		{
			playCount++;
			currentTime = fmod(currentTime, currentState->animation->getDuration());
		}
		
		calculateBoneTransform(&currentState->animation->getRootNode(), glm::mat4(1.0f));	//start with identity matrix for top level node
	}
}

void Animator::checkForTransitions()
{
	//Check all of the exit transitions for our current state to see if any of them have their condition met
	for (auto& stateTransitionPair : currentState->transitions)
	{
		auto& transitionFunction = stateTransitionPair.second;
		if (transitionFunction(this))
		{
			auto& newState = stateTransitionPair.first;
			setState(newState);
		}
	}
}

void Animator::setState(AnimationGraphNode* newState)
{
	currentState = newState;
	currentTime = 0.0f;
	playCount = 0;
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* bone = currentState->animation->findBone(nodeName);
	if (bone)
	{
		bone->update(currentTime);
		nodeTransform = bone->getLocalTransform();
	}
	glm::mat4 globalTransform = parentTransform * nodeTransform;
	auto& boneInfoMap = currentState->animation->getBoneIDMap();
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
	if (currentState->animation)
	{
		return currentState->animation->getModel();
	}
	return nullptr;
}

void Animator::setFloat(std::string key, float value)
{
	floatVals[key] = value;
}

void Animator::setBool(std::string key, bool value)
{
	boolVals[key] = value;
}