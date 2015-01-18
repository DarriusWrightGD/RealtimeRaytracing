#pragma once 
#include "Material.h"
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif
#include <glm.hpp>

typedef struct
{
	Material material;
	//float position[4];
	glm::vec4 position;
	//cl_float radius;
	inline float getMinX()
	{
		return -position[3]; 
	}

	inline float getMinY()
	{
		return  -position[3]; 
	}

	inline float getMinZ()
	{
		return -position[3]; 
	}

	inline float getMaxX()
	{
		return  +position[3]; 
	}

	inline float getMaxY()
	{
		return +position[3]; 
	}

	inline float getMaxZ()
	{
		return  +position[3]; 
	}

}Sphere;