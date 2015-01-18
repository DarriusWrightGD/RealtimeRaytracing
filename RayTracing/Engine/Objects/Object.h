#pragma once
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif
#include <BoundingBox\BBox.h>
#include <Objects\Material.h>
#include <glm.hpp>

class Object
{
public :

	BBox box;
	cl_int triangleIndex;
	cl_uint index;
	cl_uint meshIndex;
	float d [1];
};