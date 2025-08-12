#pragma once

#include <math.h>
#include <format>
#include <string>
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "DebuggingTools.h"
#include "Frustrum.h"
#include "Utilities.h"
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

	void updateFollowValuesFromTransform();

public:
	Camera();
	Camera(glm::vec3 pos, glm::vec3 dir, bool isFloating, glm::vec2 screenSize);
	~Camera();
	void pan(float delta);
	void rise(float delta);
	void forward(float delta);
	void updateDirectionFirstPerson(float dx, float dy);
	void updateFollowingPosition();
	void updateFrustrum();	//call this after done adjusting camera position/direction for a frame
	void calculateViewMatrix();
	void calculateViewMatrixNoPosition();
	void calculateProjectionMatrix(glm::vec2 screenDimensions);
	glm::mat4 getViewMatrix();
	glm::mat4 getViewMatrixNoPosition();
	glm::mat4 getProjectionMatrix();
	glm::vec3 computeRayThroughScreen(glm::vec2 pixelCoords);
	glm::vec2 worldToScreen(glm::vec3 worldPosition, bool& inFrustrum);
	int getLookDirection() const;
	std::string getMainVectorsString() const;

	Frustrum frustrum;
	Transform transform;
	glm::vec2 screenSize;	//Size of the screen to which the camera is rendering

	//Variables associated with first-person vs follow perspective
	bool followCamera;	//Other than this one, the rest of these variables only apply to a follow-type camera
	float followDistance;
	float followHeight;
	float followBearing;
	glm::vec3 followTarget;

	void switchToFollowMode();
	void switchToFirstPersonMode();
};