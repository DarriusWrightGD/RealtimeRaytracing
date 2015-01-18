#include "Camera.h"


Camera::Camera(glm::vec3 position , glm::vec3 lookAt, float distance, float radius, float focalDistance) :focalDistance(focalDistance), up(0,1,0,0), position(position,radius), lookAt(lookAt,0.0), distance(distance)
	,type(CameraType::Pinhole)
{
}


Camera::~Camera(void)
{
}


void Camera::update()
{
	w = glm::vec4(glm::normalize(glm::vec3(position - lookAt)),0.0);
	u = glm::vec4(glm::normalize(glm::cross(glm::vec3(up), glm::vec3(w))),0.0);
	v = glm::vec4(glm::cross(glm::vec3(w),glm::vec3(u)),0.0);
}

void Camera::moveLeft()
{
	position.x -= CAMERA_SPEED;
}
void Camera::moveRight()
{
	position.x += CAMERA_SPEED;

}
void Camera::moveUp()
{
	position.y += CAMERA_SPEED;

}
void Camera::moveDown()
{
	position.y -= CAMERA_SPEED;

}
void Camera::moveIn()
{
	position.z -= CAMERA_SPEED;

}
void Camera::moveOut()
{
	position.z += CAMERA_SPEED;

}
