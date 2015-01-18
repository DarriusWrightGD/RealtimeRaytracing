#pragma once
#include <Objects\Material.h>

enum LightType
{
	DIRECTIONAL_TYPE = 0,
	POINT_TYPE = 1

};


struct Light
{
	Material material;
	cl_float3 position;
	cl_float4 direction;
	LightType type;
	float d[3];
};
