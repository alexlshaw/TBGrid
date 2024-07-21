#pragma once

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

#include "DebuggingTools.h"
#include "Frustrum.h"
#include <math.h>
#include "Math.h"
#include "Transform.h"

const enum lookDirection { SOUTHWEST, NORTHWEST, NORTHEAST, SOUTHEAST };

class Camera
{
private:
	glm::mat4 viewMatrix;
	glm::mat4 viewMatrixOrigin;
	glm::mat4 projectionMatrix;
	const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float nearPlaneDistance = 0.1f;
	float farPlaneDistance = 2000.0f;
	float fieldOfView = 45.0f;
	float aspectRatio;
public:
	Camera();
	Camera(glm::vec3 pos, glm::vec3 dir, bool isFloating, int screenWidth, int screenHeight);
	~Camera();
	void pan(float delta);
	void rise(float delta);
	void forward(float delta);
	void updateDirection(float dx, float dy);
	void updateFrustrum();	//call this after done adjusting camera position/direction for a frame
	void calculateViewMatrix();
	void calculateViewMatrixNoPosition();
	void calculateProjectionMatrix(int screenWidth, int screenHeight);
	glm::mat4 getViewMatrix();
	glm::mat4 getViewMatrixNoPosition();
	glm::mat4 getProjectionMatrix();

	int getLookDirection();
	void getMainVectorsString(char* buffer);
	void getAngleString(char* buffer);

	bool floating;
	Frustrum frustrum;
	Transform transform;
};