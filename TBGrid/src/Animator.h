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

//Forward declarations for AnimationGraphTransition
class Animator;	
class AnimationGraphNode;

//our transition function should be a lambda that considers the animator and returns true if the state of the animator indicates that the animation should change
struct AnimationGraphTransition
{
	AnimationGraphNode* target;
	float transitionTime;
	std::function<bool(Animator*)> condition;
};

class AnimationGraphNode
{
public:
	Animation* animation;
	std::vector<AnimationGraphTransition> transitions;
	AnimationGraphNode(Animation* animation) : animation(animation) {}
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

	void beginAnimationTransition(AnimationGraphTransition& transition);
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
	float getFloat(const std::string key) const;
	bool getBool(const std::string key) const;
	int playCount = 0;	//number of times the current animation has completed
};