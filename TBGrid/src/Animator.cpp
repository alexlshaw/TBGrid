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
	//if we're transitioning, update progress
	if (targetTransitionAnimation)
	{
		elapsedTransitionTime += delta;
	}
	//if we have an animation, play it
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
	//Firstly, check if we're already in the process of transitioning state
	if (targetTransitionAnimation)
	{
		//We are, check if the transition should finish
		float t = elapsedTransitionTime / targetTransitionTime;
		if (t >= 1.0f)
		{
			//transition is finished, update state
			setState(targetTransitionAnimation);
		}
	}
	else
	{
		//we're not currently transitioning, figure out if we should
		//Check all of the exit transitions for our current state to see if any of them have their condition met
		for (auto& transition : currentState->transitions)
		{
			if (transition.condition(this))
			{
				beginAnimationTransition(transition);
			}
		}
	}
}

void Animator::setState(AnimationGraphNode* newState)
{
	currentState = newState;
	currentTime = 0.0f;
	playCount = 0;
	targetTransitionAnimation = nullptr;
}

void Animator::beginAnimationTransition(AnimationGraphTransition& transition)
{
	targetTransitionAnimation = transition.target;
	elapsedTransitionTime = 0.0f;
	targetTransitionTime = transition.transitionTime;
}

glm::mat4 Animator::computeTransitionTransform(Bone* bone)
{
	//1. Compute transition progress (lerp factor)
	float t = elapsedTransitionTime / targetTransitionTime;	//we're going to handle the actual transition elsewhere, here we assume that t < 1.0
	//2. acquire positions of the bone when we started transitioning (lerp start)
	//		No actual action required, we access them directly on the bone further down
	//3. compute target positions from target animation keyFrame 0 (lerp end)
	Bone* targetBone = targetTransitionAnimation->animation->findBone(bone->getBoneName());
	if (targetBone)
	{
		glm::vec3 targetTranslation = targetBone->getKeyTranslation(0);
		glm::quat targetRotation = targetBone->getKeyRotation(0);
		glm::vec3 targetScale = targetBone->getKeyScale(0);
		//4. interpolate them
		glm::vec3 finalTranslation = glm::mix(bone->currentPosition, targetTranslation, t);
		glm::quat finalRotation = glm::slerp(bone->currentRotation, targetRotation, t);
		glm::vec3 finalScale = glm::mix(bone->currentScale, targetScale, t);
		//5. Bring it all together and return the final transformation matrix
		return glm::scale(glm::translate(glm::mat4(1.0f), finalTranslation) * glm::mat4_cast(finalRotation), finalScale);
	}
	DEBUG_PRINTLN(std::format("Couldn't find matching bone: {} in target transition animation", bone->getBoneName()));
	return glm::mat4(1.0f);
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
{
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* bone = currentState->animation->findBone(nodeName);
	if (bone)
	{
		if (targetTransitionAnimation)
		{
			//we have a target animation, we want to stop updating the bones of the current animation so that the start of our lerp is their current position
			//instead just calculate our target transformation and proceed
			nodeTransform = computeTransitionTransform(bone);
		}
		else
		{
			bone->update(currentTime);
			nodeTransform = bone->getLocalTransform();
		}
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