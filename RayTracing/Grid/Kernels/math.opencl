#pragma OPENCL EXTENSION cl_khr_fp64: enable
__constant float epsilion = 0.00001f;
__constant float airIndex = 1.000293f;
__constant uint DIRECTIONAL_LIGHT = 0;
__constant uint POINT_LIGHT = 1;
__constant int TRIANGLE = 0;
__constant int SPHERE = 0;
__constant uint PINHOLE_CAMERA = 0;
__constant uint THINLENS_CAMERA = 1;
__constant uint FISHEYE_CAMERA = 2;
__constant uint SPHERICAL_CAMERA = 3;
__constant uint STEREO_CAMERA = 4;
__constant uint DIFFUSE = 0;
__constant uint SPECULAR = 1;
__constant uint TRANS = 2;
__constant int OCTREE_MAX_OBJECTS = 5;
__constant float PI = 3.1415926535f;//...
__constant float PI_1 = 1.0f/3.1415926535f;
long inline nextPowerOfTwo(long number)
{
	return pow(2.0, ceil(log((double)number)/log(2.0)));
}
float absF(float x)
{
	return x < 0 ? x * -1.0f : x;
}
float3 mapToHemiSphere(float2 samplePoint, float e)
{
	float cosPhi = cos(2.0f * PI * samplePoint.x);
	float sinPhi = sin(2.0f * PI * samplePoint.x);
	float cosTheata = pow((1.0f - samplePoint.y), 1.0f / (e + 1.0f)) ;
	float sinTheata = sqrt(1.0f - cosTheata * cosTheata) ;

	return (float3)(sinTheata * cosPhi, sinTheata * sinPhi, cosTheata);

}
uint3 mixUIntColor(uint3 leftColor, uint3 rightColor)
{
	float3 colorf = convert_float3(leftColor.xyz) / 255.0f;
	float3 colorf2 = convert_float3(rightColor.xyz) / 255.0f;
	float3 finalColor = (colorf * colorf2) * 255.0f;

	return convert_uint3(finalColor);
}
float2 mapToDisk(float2 samplePoint)
{
	float r,phi;

	if(samplePoint.x > -samplePoint.y)
	{
		if(samplePoint.x > samplePoint.y)
		{
			r = samplePoint.x;
			phi = samplePoint.y/ samplePoint.x;
		}
		else
		{
			r = samplePoint.y;
			phi = 2 - samplePoint.x/ samplePoint.y;
		}
	}
	else
	{
		if(samplePoint.x < samplePoint.y)
		{
			r = -samplePoint.x;
			phi = 4 + samplePoint.y/ samplePoint.x;
		}
		else
		{
			r = -samplePoint.y;
			if(samplePoint.y != 0.0f)
				phi = 6 - samplePoint.x/samplePoint.y;
			else
				phi = 0.0f;
		}
	}
	phi *= PI/4.0f;

	return (float2)(r* cos(phi),r * sin(phi));
}

uint3 convertColor(float4 color)
{
	return convert_uint3(color.xyz * 255.0f);
}

float clampFloat(float x)
{
	return clamp(x,0.0f,1.0f);
}

uint toUInt(float x)
{
	return clampFloat(x) * 255;//(uint)(pow(clampFloat(x),1.0f/2.2f) * 255 + .5);
}


uint4 toRGBA(float4 color)
{
	return (uint4)(toUInt(color.x),toUInt(color.y),toUInt(color.z),toUInt(color.w));
}

float3 reflect(float3 direction, float3 normal)
{
	return direction - normal * 2.0f * dot(normal, direction);
}