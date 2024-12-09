#pragma once

#include "Animation.h"
#include "assimp/scene.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace AnimationConstants
{
	constexpr int MAX_BONES = 100;	//Has a matching value in the skeletal animation shader, don't change one without changing both
}

class Animator;	//Forward declare for the state transition function in AnimationGraphNode

//our transition function should be a lambda that considers the animator and returns true if the state of the animator indicates that the animation should change
//TODO: Because AnimationGraphs are frequently circular, the Animator should explicitly release the graph nodes in its destructor
//	--What I probably want to do is to give the animator a vector of all possible states it can be in, then release all that in the destructor
class AnimationGraphNode
{
public:
	Animation* animation;
	float exitTransitionTime;	//TODO: This should really be per-exitState, rather then apply to this state as a whole
	std::vector<std::pair<AnimationGraphNode*, std::function<bool(Animator*)>>> transitions;	//We use a raw ptr here so that the graph nodes don't prevent each others destruction
	AnimationGraphNode(Animation* animation, float exitTime) 
		: animation(animation), 
		exitTransitionTime(exitTime) 
	{}
};

//This class directs animations to play and represents the animation graph
class Animator
{
private:
	std::map<std::string, float> floatVals;
	std::map<std::string, bool> boolVals;
	std::vector<glm::mat4> finalBoneMatrices;
	float currentTime = 0.0f;
	void checkForTransitions();
	void setState(AnimationGraphNode* newState);
	AnimationGraphNode* currentState;
	std::vector<std::shared_ptr<AnimationGraphNode>> allStates;

	void beginAnimationTransition(AnimationGraphNode* newState, float transitionTime);
	AnimationGraphNode* targetTransitionAnimation = nullptr;
	float elapsedTransitionTime = 0.0f;
	float targetTransitionTime = 1.0f;
	glm::mat4 computeTransitionTransform(Bone* bone);
public:
	Animator(AnimationGraphNode* startingState, std::vector<std::shared_ptr<AnimationGraphNode>> stateGraph);
	void updateAnimation(float delta);
	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	std::vector<glm::mat4>& getFinalBoneMatrices();
	AnimatedModel* getModel();
	void setFloat(std::string key, float value);
	void setBool(std::string key, bool value);
	int playCount = 0;	//number of times the current animation has completed
};