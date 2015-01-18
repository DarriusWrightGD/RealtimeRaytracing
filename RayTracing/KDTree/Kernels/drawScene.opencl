

/*

Octree findBaseNode(Octree * treeNodes, Octree node,Ray ray, float t1)
{


	if(node.childrenIndices[0] == -1)
	{
		HitReturn hitRet = hitBox(ray,node.boundingBox );

	}
	else
	{
		Octree currentNode;
		for(int i = 0; i < 8; i++)
		{
			currentNode = treeNodes[node.childrenIndices[i]];
			if(currentNode.numberOfObjects != 0)
			{
				HitReturn hitRet = hitBox(ray,currentNode.boundingBox )
				if(hitRet.hit && hitRet.t1 < t1 && currentNode.numberOfObjects != 0 && currentNode.childrenIndices[i] != -1)
				{
					t1 = hitRet.min;
				}
			}

		}
	}

	
}

*/

/*
__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, 
	sampler_t sampler, int width, int height, __global Object * objects,
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global Octree * treeNodes
	Camera camera
	)
{
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	Ray ray = generateRay(outImageCoord, width, height, camera);
	uint4 outColor = getColor();

	HitReturn hitCheck = hitBBox(ray,box.min,box.max);

	if(hitCheck.hit)
	{
		
		for(int i = 0; i < 8; i++)
		{
			Octree currentNode = treeNodes[treeNodes[0].childrenIndices[i]];
			if(hitBBox, currentNode.boundingBox.min,currentNode.boundingBox.max )
			{
				bool complete = false;
				do
				{
					for(int i = 0; i < 8; i++)
					{

					}
				}while()
			}
		}

	}
	write_imageui(dstImage, outImageCoord, outColor);
}

*/


__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, 
	sampler_t sampler, int width, int height, __global Object * objects,
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, int nx, int ny, int nz,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera
	)
{
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	Ray ray = generateRay(outImageCoord, width, height, camera);
	uint4 outColor = getColor();

	HitReturn hitCheck = hitBBox(ray,box.min,box.max);

	if(hitCheck.hit)
	{
		
		int3 currentCell;
		float3 cellDimensions = (float3)(nx,ny,nz);
		if(insideBBox(ray.origin,box.min,box.max ))
		{
			currentCell = (int3)(convert_int3(clamp((ray.origin - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
		}
		else
		{
			float3 p = ray.origin + (hitCheck.minValue * ray.direction);
			currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
		}
		int cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;

		
		float3 delta = (box.max - box.min)/cellDimensions ;
		//float3 next = (float3)(box.min.x + (currentCell.x + 1) * delta.x,box.min.y + (iy + 1) * delta.y,box.min.z + (iz + 1) * delta.z);
		float3 next = (convert_float3(currentCell) + 1.0f) * delta;
		bool run =  true;
		
		while(run)
		{
			int cellObjectNumber = cellIndices[cellIndex]- cellIndices[cellIndex-1];
			//cellObjectNums[cellIndex] = cellObjectNumber;
			float lastDistance = 100000000.0f;
			for(int i = 0; i <  cellObjectNumber; i++ )
			{
				int objectIndex = objectIndices[cellIndices[cellIndex-1] + i]-1;

				if(objects[objectIndex].position.w > 0.0)
				{
					SphereInfo sphereInfo = sphereIntersection(ray,objects[objectIndex].position.xyz,objects[objectIndex].position.w);
					if(sphereInfo.hasIntersection && sphereInfo.distanceToIntersection < lastDistance)
					{
						outColor = (uint4)adsLight(objects[objectIndex], light[0], sphereInfo);
						lastDistance = sphereInfo.distanceToIntersection;
						run = false;

					}
				}
			}
			if(run)
			{
				if(next.x  < next.y && next.x < next.z)
				{
					next.x += delta.x;
					currentCell.x += 1;
					run = (currentCell.x != cellDimensions.x);
					
				}
				else
				{
					if(next.y < next.z)
					{
						next.y += delta.y;
						currentCell.y += 1;
						run = (currentCell.y != cellDimensions.y);
					}
					else
					{
						next.z += delta.z;
						currentCell.z += 1;
						run = (currentCell.z != cellDimensions.z);
					}
				
				}
				cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
			}
		}


	}
	write_imageui(dstImage, outImageCoord, outColor);

}



/*
__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, 
	sampler_t sampler, int width, int height, __global Object * objects,
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, int nx, int ny, int nz,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera
	)
{
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	Ray ray = generateRay(outImageCoord, width, height,camera);
	uint4 outColor = getColor();
HitReturn hitCheck = hitBBox(ray,box.min,box.max);

	if(hitCheck.hit)
	{

		for(int index = 0; index < numberOfObjects; index++)
		{

			if(objects[index].position.w > 0.0)
			{
				SphereInfo sphereInfo = sphereIntersection(ray,objects[index].position.xyz,objects[index].position.w);
				if(sphereInfo.hasIntersection)
				{
					outColor = (uint4)adsLight(objects[index], light[0], sphereInfo);
				}
			}
		}
	}
	write_imageui(dstImage, outImageCoord, outColor);

}

*/