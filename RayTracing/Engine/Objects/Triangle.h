#pragma once
#include <glm.hpp>


struct Triangle
{
public:
	glm::vec3 v0;
	float d0;
	glm::vec3 v1;
	float d1;
	glm::vec3 v2;
	float d2;
	
	glm::vec3 normal;
	float d3;
};

