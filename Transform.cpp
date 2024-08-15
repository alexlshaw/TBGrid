#include "Transform.h"

Transform::Transform()
	: position(glm::vec3(0.0f, 0.0f, 0.0f)), rotation(glm::identity<glm::mat4>()), scale(glm::vec3(1.0f, 1.0f, 1.0f))
{
	buildTransformMatrix();
}

Transform::Transform(glm::vec3 position, glm::mat4 rotation, glm::vec3 scale)
	: position(position), rotation(rotation), scale(scale)
{
	buildTransformMatrix();
}

void Transform::buildTransformMatrix()
{
	//takes the individual transformation properties of the object and combines them to form the transformation matrix
	transform = glm::identity<glm::mat4>();
	transform = glm::translate(transform, position);
	transform *= rotation;
	transform = glm::scale(transform, scale);

	normal = glm::inverseTranspose(glm::mat3(transform));
	dirty = false;
}

glm::vec3 Transform::getPosition() const
{
	return position;
}

void Transform::setPosition(glm::vec3 pos)
{
	dirty = true;
	position = pos;
}

glm::vec3 Transform::getScale() const
{
	return scale;
}

void Transform::setScale(glm::vec3 scl)
{
	dirty = true;
	scale = scl;
}

glm::mat4 Transform::getRotation() const
{
	return rotation;
}

void Transform::setRotation(glm::mat4 rot)
{
	dirty = true;
	rotation = rot;
}

void Transform::setRotationFromAngles(glm::vec4 rot)
{
	rotation = glm::rotate(glm::identity<glm::mat4>(), rot.w, glm::vec3(rot.x, rot.y, rot.z));
	dirty = true;
}

glm::mat4 Transform::getMatrix()
{
	if (dirty)
	{
		buildTransformMatrix();
	}
	return transform;
}

glm::mat3 Transform::getNormalMatrix()
{
	if (dirty)
	{
		buildTransformMatrix();
	}
	return normal;
}

void Transform::lookAt(glm::vec3 targetPosition)
{
	//TODO: revisit this when using this function to orient objects
	//Note that position and targetPosition are swapped here: we're generating the opposite look vector that we would expect to
	rotation = glm::lookAt(targetPosition, position, glm::vec3(0.0f, 1.0f, 0.0f));
	dirty = true;
}

glm::vec3 Transform::getForward() const
{
	glm::mat4 inverted = glm::inverse(rotation);
	return normalize(glm::vec3(inverted[2]));
}

Transform Transform::stackTransforms(Transform other)
{
	glm::mat4 outputMatrix = this->getMatrix() * other.getMatrix();
	Transform output;
	output.setMatrix(outputMatrix);
	return output;
}

void Transform::setMatrix(glm::mat4 matrix)
{
	transform = matrix;
	normal = glm::inverseTranspose(glm::mat3(transform));
	dirty = false;
}