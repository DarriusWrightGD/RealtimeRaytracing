#pragma once
#include <glm.hpp>
#include<ExportHeader.h>
static const float CAMERA_SPEED = 0.2f;

enum CameraType
{
	Pinhole = 0,
	Thinlens,
	FishEye,
	Spherical,
	Stero

};


class Camera
{
public:
	ENGINE_SHARED Camera(glm::vec3 position = glm::vec3(0,0,0) , glm::vec3 lookAt = glm::vec3(0,0,-1), float distance = 200.0f, float radius = 0.4f ,float focalDistance = 270.0f);
	ENGINE_SHARED ~Camera(void);
	ENGINE_SHARED void moveLeft();
	ENGINE_SHARED void moveRight();
	ENGINE_SHARED void moveUp();
	ENGINE_SHARED void moveDown();
	ENGINE_SHARED void moveIn();
	ENGINE_SHARED void moveOut();
	ENGINE_SHARED void update();

	glm::vec4 u;
	glm::vec4 v;
	glm::vec4 w;
	glm::vec4 up;
	glm::vec4 lookAt;
	glm::vec4 position;
	float distance;
	float zoom;
	CameraType type;
	float focalDistance;
	
	




};

