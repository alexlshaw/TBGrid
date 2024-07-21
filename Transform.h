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
public:
	Transform();
	glm::vec3 getPosition() const;
	void setPosition(glm::vec3 pos);
	glm::vec3 getScale() const;
	void setScale(glm::vec3 scl);
	glm::mat4 getRotation() const;
	void setRotation(glm::mat4 rot);
	void setRotationFromAngles(glm::vec4 rot);
	glm::mat4 getMatrix();
	glm::mat3 getNormalMatrix();
	void lookAt(glm::vec3 pos);		//sets the rotation matrix such that the transform is oriented to have its forward vector pointing to the desired spot (after considering position)
	glm::vec3 getForward() const;			//gets the position-independent forward vector of the transform
};