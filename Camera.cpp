#include "Camera.h"

#include <format>

Camera::Camera()
	:	aspectRatio(1.0f),
		projectionMatrix(glm::identity<glm::mat4>()),
		viewMatrix(glm::identity<glm::mat4>()),
		viewMatrixOrigin(glm::identity<glm::mat4>()),
		followCamera(false),
		followDistance(7.0f),
		followHeight(4.5f),
		followTarget(glm::vec3(0.0f, 0.0f, 0.0f)),
		followBearing(0.0f),
		screenSize(glm::vec2(1.0f, 1.0f))
{}

Camera::Camera(glm::vec3 pos, glm::vec3 dir, bool isFloating, glm::vec2 screenSize)
	:	followCamera(true),
		followDistance(7.0f),
		followHeight(4.5f),
		followTarget(glm::vec3(0.0f, 0.0f, 0.0f)),
		followBearing(0.0f),
		screenSize(screenSize)
{
	transform.setPosition(pos);
	transform.lookAt(pos + dir);
	aspectRatio = screenSize.x / screenSize.y;
	frustrum.setCamInternals(fieldOfView, aspectRatio, nearPlaneDistance, farPlaneDistance);
	frustrum.setCamDef(transform.getPosition(), transform.getPosition() + transform.getForward(), up);
	calculateProjectionMatrix(screenSize);
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

	if (followCamera)
	{
		updateFollowValuesFromTransform();
	}
}

//Moves the camera in the xz plane along its forward vector
void Camera::forward(float delta)
{
	glm::vec3 position = transform.getPosition();
	glm::vec3 forward = transform.getForward();
	glm::vec3 horizontalDirection = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
	position += horizontalDirection * delta;
	transform.setPosition(position);

	if (followCamera)
	{
		updateFollowValuesFromTransform();
	}
}

//Moves the camera up or down. Not affected by camera's rotation
void Camera::rise(float delta)
{
	glm::vec3 position = transform.getPosition();
	position.y += delta;
	transform.setPosition(position);

	if (followCamera)
	{
		updateFollowValuesFromTransform();
	}
}

void Camera::updateDirectionFirstPerson(float dx, float dy)
{
	glm::mat4 rotation = transform.getRotation();
	rotation = glm::rotate(rotation, -dx, glm::vec3(0.0f, 1.0f, 0.0f));
	transform.setRotation(rotation);	//set rotation here for convenience of getting forward vector below
	rotation = glm::rotate(rotation, dy, glm::cross(up, transform.getForward()));
	transform.setRotation(rotation);
}

void Camera::updateFollowValuesFromTransform()
{
	//we need to compute a new follow target based on our current position
	glm::vec3 forward = transform.getForward();
	glm::vec3 offset = forward * followDistance;
	followTarget = transform.getPosition() + offset;
	followHeight = -offset.y;

	//compute new bearing around the target
	followBearing = (RADIANS_TO_DEGREES * atan2(offset.x, offset.z)) + 90.0f;
	if (followBearing < 0.0f)
	{
		followBearing += 360.0f;
	}
	else if (followBearing > 360.0f)
	{
		followBearing -= 360.0f;
	}
}

void Camera::updateFollowingPosition()
{
	//start by computing the following offset vector

	//We consider the camera to be fixed on a circle that floats [height] units above the target
	//The radius of the circle is dependent on the distance the camera is supposed to stay at, after taking into consideration the height
	//the camera's position on the circle is determined by [bearing] in the 0-360 degree range where 0 indicates the camera lies directly along the x-axis from the target

	float radius = sqrt((followDistance * followDistance) - (followHeight * followHeight));
	float z = radius * -sin(DEGREES_TO_RADIANS * followBearing);
	float x = radius * cos(DEGREES_TO_RADIANS * followBearing);
	glm::vec3 offsetVector(x, followHeight, z);

	transform.setPosition(followTarget + offsetVector);
	transform.lookAt(followTarget);
}

void Camera::updateFrustrum()
{
	frustrum.setCamDef(transform.getPosition(), transform.getPosition() + transform.getForward(), up);
}

void Camera::calculateProjectionMatrix(glm::vec2 screenDimensions)
{
	// Projection matrix : 45° Field of View, aspect ratio determined by resolution, display range : 0.1 unit <-> 2000 units
	projectionMatrix = glm::perspective(45.0f, screenDimensions.x / screenDimensions.y, nearPlaneDistance, farPlaneDistance); //for when the view angle gradually changes
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

//Compute the direction of a ray going from the camera through a particular pixel in the view plane
glm::vec3 Camera::computeRayThroughScreen(glm::vec2 pixelCoords)
{
	float mX = pixelCoords.x / (screenSize.x * 0.5f) - 1.0f;	//mouse X in range -1...1
	float mY = pixelCoords.y / (screenSize.y * 0.5f) - 1.0f;	//mouse Y in range -1...1
	glm::mat4 inverseVP = glm::inverse(getProjectionMatrix() * getViewMatrix());
	glm::vec4 screenPos = glm::vec4(mX, -mY, 1.0f, 1.0f);
	glm::vec4 worldPos = inverseVP * screenPos;
	return glm::normalize(glm::vec3(worldPos));
}

int Camera::getLookDirection() const
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

void Camera::switchToFollowMode()
{
	followCamera = true;
	updateFollowValuesFromTransform();
}

void Camera::switchToFirstPersonMode()
{
	//swapping to FPP is very simple as we have no extra variables to track
	followCamera = false;
}