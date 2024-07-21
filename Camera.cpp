#include "Camera.h"

#include <format>

Camera::Camera()
{
	//no need to set position and direction, transform should have appropriate defaults
	floating = false;
	aspectRatio = 1.0f;
	projectionMatrix = glm::identity<glm::mat4>();
	viewMatrix = glm::identity<glm::mat4>();
	viewMatrixOrigin = glm::identity<glm::mat4>();
}

Camera::Camera(glm::vec3 pos, glm::vec3 dir, bool isFloating, int screenWidth, int screenHeight)
{
	transform.setPosition(pos);
	transform.lookAt(pos + dir);
	floating = isFloating;
	aspectRatio = float(screenWidth) / float(screenHeight);
	frustrum.setCamInternals(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);
	frustrum.setCamDef(transform.getPosition(), transform.getPosition() + transform.getForward(), up);
	calculateProjectionMatrix(screenWidth, screenHeight);
}

Camera::~Camera()
{
}

//Moves the camera along the (camera's) x axis
void Camera::pan(float delta)
{
	glm::vec3 right = glm::normalize(glm::cross(transform.getForward(), up));
	glm::vec3 position = transform.getPosition();
	position -= right * delta;
	transform.setPosition(position);
}

//Moves the camera in the xz plane along its forward vector
void Camera::forward(float delta)
{
	glm::vec3 position = transform.getPosition();
	glm::vec3 forward = transform.getForward();
	glm::vec3 horizontalDirection = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
	position += horizontalDirection * delta;
	transform.setPosition(position);
}

//Moves the camera up or down. Not affected by camera's rotation
void Camera::rise(float delta)
{
	glm::vec3 position = transform.getPosition();
	position.y += delta;
	transform.setPosition(position);
}

void Camera::updateDirection(float dx, float dy)
{
	glm::mat4 rotation = transform.getRotation();
	rotation = glm::rotate(rotation, -dx, glm::vec3(0.0f, 1.0f, 0.0f));
	transform.setRotation(rotation);	//set rotation here for convenience of getting forward vector below
	rotation = glm::rotate(rotation, dy, glm::cross(up, transform.getForward()));
	transform.setRotation(rotation);
}

void Camera::updateFrustrum()
{
	frustrum.setCamDef(transform.getPosition(), transform.getPosition() + transform.getForward(), up);
}

void Camera::calculateProjectionMatrix(int screenWidth, int screenHeight)
{
	// Projection matrix : 45° Field of View, aspect ratio determined by resolution, display range : 0.1 unit <-> 2000 units
	projectionMatrix = glm::perspective(45.0f, float(screenWidth) / float(screenHeight), nearPlaneDistance, farPlaneDistance); //for when the view angle gradually changes
}

void Camera::calculateViewMatrix()
{
	viewMatrix = glm::lookAt(transform.getPosition(), transform.getPosition() + transform.getForward(), up);
}

void Camera::calculateViewMatrixNoPosition()
{
	viewMatrixOrigin =  glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), transform.getForward(), up);
}

glm::mat4 Camera::getViewMatrix()
{
	return viewMatrix;
}

glm::mat4 Camera::getViewMatrixNoPosition()
{
	return viewMatrixOrigin;
}

glm::mat4 Camera::getProjectionMatrix()
{
	return projectionMatrix;
}

int Camera::getLookDirection()
{
	glm::vec3 direction = transform.getForward();
	float f = atan2(direction.x, direction.z);
	int result = NORTHEAST;
	if (f >= 0.0f && f < PI / 2)
	{
		result = NORTHWEST;
	}
	else if (f >= PI / 2 && f < 2 * PI)
	{
		result = SOUTHWEST;
	}
	else if (f >= -2 * PI && f < -PI / 2)
	{
		result = SOUTHEAST;
	}

	return result;
}

void Camera::getMainVectorsString(char* buffer)
{
	glm::vec3 position = transform.getPosition();
	glm::vec3 direction = transform.getForward();
	sprintf_s(buffer, 128, "Position: [x:%f, y:%f, z:%f], Direction: [x:%f, y:%f, z:%f]", position.x, position.y, position.z, direction.x, direction.y, direction.z);
}

void Camera::getAngleString(char* buffer)
{
	char dirStr[17];
	int direction = getLookDirection();
	if (direction == NORTHEAST)
	{
		sprintf_s(dirStr, "Facing Northeast");
	}
	else if (direction == NORTHWEST)
	{
		sprintf_s(dirStr, "Facing Northwest");
	}
	else if (direction == SOUTHEAST)
	{
		sprintf_s(dirStr, "Facing Southeast");
	}
	else if (direction == SOUTHWEST)
	{
		sprintf_s(dirStr, "Facing Southwest");
	}
	sprintf_s(buffer, 64, "vert: %f, hor: %f, %s", 0.0f, 0.0f, dirStr);
}