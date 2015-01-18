#pragma once

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include <glm.hpp>

class BBox
{
public :
	glm::vec3 min;
	float d1;
	glm::vec3 max;
	float d2;

	float surfaceArea()
	{
		glm::vec3 d = max - min;
		return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
	}

	void expand(BBox box)
	{
		min  = glm::min(box.min, min);
		max = glm::max(box.max, max);
	}

	float volumne()
	{
		glm::vec3 d = max - min;
		return d.x * d.y * d.z;
	}

	int maxExtent()
	{
		glm::vec3 d = max - min;
		return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z) ? 1 : 2;
	}


};