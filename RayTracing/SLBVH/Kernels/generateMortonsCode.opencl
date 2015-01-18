unsigned int part(unsigned int n)
{
	
	n = (n ^ (n<<16))& 0xff0000ff;
	n = (n ^ (n<<8)) & 0x0300f00f;
	n = (n ^ (n<<4)) & 0x030c30c3;
	n = (n ^ (n<<2)) & 0x09249249;

	return n;
}


unsigned int mCode (unsigned int x,unsigned int y,unsigned int z)
{
	return (part(z) << 2) +(part(y) << 1) + part(x);
}


__kernel void initializeMortonCodes(__global Object * objects, __global MortonNode *  codes//,
	)//__global float3 * positions)
{
	int objectIndex = get_global_id(0);
	codes[objectIndex].index = objectIndex;

	float3 position = objects[objectIndex].position.xyz;
	uint3 convertedPosition =  convert_uint3(position);
//	positions[objectIndex] = position;

	codes[objectIndex].code = mCode(convertedPosition.x,convertedPosition.y,convertedPosition.z );
}