#pragma once
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif

#include "Material.h"

struct Mesh
{
	Material material;
	cl_float4 position;
	cl_uint2 indices;
	float d[2];
};