#pragma once

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\matrix_inverse.hpp"

class Transform
{
private:
	glm::vec3 position;
	glm::mat4 rotation;		//Storing rotation as a matrix because it's complex. Could be improved by using quaternion
	glm::vec3 scale;
	glm::mat4 transform;	//The full transformation represented by the class
	glm::mat3 normal;
	bool dirty;						//set whenever one of the individual properties is changed
	void buildTransformMatrix();
protected:
	void setMatrix(glm::mat4 matrix);		//Sets the transformation matrix (WITHOUT UPDATING RECORDED TRANSLATE, ROTATE, SCALE -> don't rely on these values)
public:
	Transform();
	Transform(glm::vec3 position, glm::mat4 rotation, glm::vec3 scale);
	glm::vec3 getPosition() const;
	void setPosition(glm::vec3 pos);
	glm::vec3 getScale() const;
	void setScale(glm::vec3 scl);
	glm::mat4 getRotation() const;
	void setRotation(glm::mat4 rot);
	void setRotationFromAngles(glm::vec4 rot);
	glm::mat4 getMatrix();
	glm::mat3 getNormalMatrix();
	glm::mat4 getInverseTransformationMatrix();
	void lookAt(glm::vec3 pos);		//sets the rotation matrix such that the transform is oriented to have its forward vector pointing to the desired spot (after considering position)
	void setForward(const glm::vec3 direction);	//Makes the transform face a certain direction
	glm::vec3 getForward() const;			//gets the position-independent forward vector of the transform
	//Creates a transform based on multiplying the matrices of two transforms together. THIS NEW TRANSFORM WILL NOT HAVE REAL TRANSLATE, ROTATE, SCALE VALUES, USE WITH CAUTION
	//More specifically, we should only use this for temporary transforms where we care about the aggregate result but not the individual values
	//We don't bother setting the individual values to save calculations that the primary use case of thus function doesn't require
	Transform stackTransforms(Transform other);
	static Transform defaultTransform();
};