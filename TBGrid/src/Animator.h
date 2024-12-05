#pragma once

#include "Animation.h"
#include "assimp/scene.h"
#include <vector>

//This class handles the playing of an animation. I think eventually it will grow to include the full animation graph

namespace AnimationConstants
{
	constexpr int MAX_BONES = 100;	//Has a matching value in the skeletal animation shader, don't change one without changing both
}


class Animator
{
private:
	std::vector<glm::mat4> finalBoneMatrices;
	Animation* currentAnimation;
	float currentTime = 0.0f;
	float deltaTime = 0.0f;
public:
	Animator(Animation* animation);
	void updateAnimation(float delta);
	void playAnimation(Animation* animation);
	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
	std::vector<glm::mat4>& getFinalBoneMatrices();
	AnimatedModel* getModel();
};