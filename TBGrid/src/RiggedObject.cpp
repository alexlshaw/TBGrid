#include "RiggedObject.h"

RiggedObject::RiggedObject(std::shared_ptr<Animator> animator)
	: animator(animator)
{
	castsShadows = true;
}

void RiggedObject::draw(size_t meshIndex)
{
	AnimatedModel* model = animator->getModel();
	model->draw(meshIndex);
}

void RiggedObject::update(float deltaTime)
{
	animator->updateAnimation(deltaTime);
}

size_t RiggedObject::getMeshCount()
{
	AnimatedModel* model = animator->getModel();
	if (model)
	{
		return model->meshes.size();
	}
	return size_t(0);
}

void RiggedObject::setRigInShader(Shader* shader)
{
	auto& boneMatrices = animator->getFinalBoneMatrices();
	shader->setUniform("finalBoneMatrices", boneMatrices);
}

Material* RiggedObject::getMaterial(size_t idx)
{
	AnimatedModel* model = animator->getModel();
	if (model && idx < model->materials.size())
	{
		return model->materials[idx];
	}
	return nullptr;
}
