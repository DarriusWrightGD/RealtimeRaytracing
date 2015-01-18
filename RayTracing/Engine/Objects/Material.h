#pragma once
#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif


enum MaterialType
{
	DIFFUSE,
	SPECULAR,
	TRANS
};

typedef struct 
{
	cl_float4 color;
	float reflection;
	float refraction;
	MaterialType type;
	float d[1];
}Material;