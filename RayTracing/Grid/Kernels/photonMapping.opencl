typedef struct 
{
	float3 position;
	float3 direction;
	float3 energy;
}Photon;


float3 randomFloat3(int seed)
{
	return (float3)(randFloat(seed) * 2.0f- 1.0f, randFloat(seed * seed) * 2.0f- 1.0f, randFloat(seed * seed  * seed) * 2.0f - 1.0f);
}

__kernel void clearPhotons(__global int * objectPhotonCount)
{
	objectPhotonCount[get_global_id(0)] = 0;
}

//findStepInfo(int3 currentCell,Ray ray, float minVal, 
//	float maxVal,float3 voxelWidth,BBox box, float3 numberOfVoxels)



float3 filterColor(float3 colorIn, float3 filterColorIn)
{
	return min(colorIn,filterColorIn);
}

float3 getPhotonColor(float3 colorIn)
{
	return filterColor(colorIn, (float3)(1,1,1));
}

 //rgb = mul3c (getColor(rgb,gType,gIndex), 1.0/sqrt(bounces));
//float[] getColor(float[] rgbIn, int type, int index){ //Specifies Material Color of Each Object
//  if      (type == 1 && index == 0) { return filterColor(rgbIn, 0.0, 1.0, 0.0);}
//  else if (type == 1 && index == 2) { return filterColor(rgbIn, 1.0, 0.0, 0.0);}
//  else                              { return filterColor(rgbIn, 1.0, 1.0, 1.0);}
//}


Photon storePhoton(int photonsPerObject,//__global Photon * photons,
	__global int * objectPhotonCount, 
	IntersectionInfo intersectInfo,int  numberOfBounces, float3 color)
{
	// position, direction, energy
	Photon photon;
	photon.position = intersectInfo.position;
	photon.direction = intersectInfo.direction;
	photon.energy = getPhotonColor(color)  * (1.0f/sqrt((float)numberOfBounces));
	//photons[(objectIndex * photonsPerObject) + atomic_inc(objectPhotonCount[intersectInfo.objectIndex])] = photon;

	return photon;
}


void drawPhoton(Photon photon, __write_only image2d_t dstImage, int width, int height)
{
	int2 outImageCoord;
 	outImageCoord.x = (width/2) + (int)(width *  photon.position.x/photon.position.z); //Project 3D Points into Scene
    outImageCoord.y = (height/2) + (int)(height * -photon.position.y/photon.position.z); //Don't Draw Outside Image	
  
    if(outImageCoord.x < width && outImageCoord.y < height &&outImageCoord.x >= 0 && outImageCoord.y >= 0 )
    {
    	uint4 outColor = (uint4)(convert_uint3(photon.energy * 255.0f), 255.0f);
		write_imageui(dstImage, outImageCoord, outColor);
	}
}


__kernel void emitPhotons(int photonSeed, int photonsPerObject,__global int * objectPhotonCount,
__global Light * light,int numberOfBounces,
BBox box, __global int * cellIndices , __global int * objectIndices,__global Object * objects,
__write_only image2d_t dstImage, float3 voxelWidth, float3 cellDimensions, int width, int height, Camera camera,
__global Triangle * triangles)
{
	int bounces = 0;
	int photonIndex = get_global_id(0);
	
	float3 color = (float3)(1.0,1.0,1.0);
	float3 rayDirection = randomFloat3(photonSeed + photonIndex);
	

	float3 previousPoint = light[0].position;
	Ray ray;
	ray.direction = normalize(rayDirection);
	ray.direction.xy = -ray.direction.xy;
	ray.origin = previousPoint;

	//if(abs(previousPoint[0] ))

	HitReturn hitCheck = hitBBox(ray,box.min,box.max);
	
	float3 intersectionPoint;
	int cellIndex;
	while(hitCheck.hit &&bounces <= numberOfBounces)
	{

		float minVal = 0.0f;
		float maxVal = 0.0f;
		int3 currentCell;
		
		if(!insideBBox(ray.origin.xyz,box.min,box.max ))
		{
			float3 p = ray.origin.xyz + (hitCheck.minValue * ray.direction.xyz);
			minVal = hitCheck.minValue;
			maxVal = hitCheck.maxValue;
		    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
		}
		else
		{
			currentCell = (int3)(convert_int3(clamp((ray.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
			maxVal = hitCheck.maxValue;

		}	
		cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
		
		IntersectionInfo intersectInfo;
		// = rayTrace(ray, camera,cellIndices, objectIndices, light, objects, triangles, cellIndex, currentCell, box, voxelWidth ,cellDimensions);
		
		if(intersectInfo.distance == -1)bounces = numberOfBounces + 1;
		intersectionPoint = intersectInfo.position;
		Photon photon = storePhoton(photonsPerObject, objectPhotonCount,intersectInfo, numberOfBounces, color);
		//draw photon

		cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
		drawPhoton(photon , dstImage, width, height);

		//shadow

		//reflect

		//raytrace

		//set the new origin

		//increment bounces
		bounces++;
	}
	//	if(photonIndex == 0)
//			write_imageui(dstImage, (int2)(200, 200), (uint4)(0,0,255,255));


}