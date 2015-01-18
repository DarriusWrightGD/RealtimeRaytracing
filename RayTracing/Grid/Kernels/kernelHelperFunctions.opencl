int3 positionToVoxel(float3 position,  float3 invWidth, float3 numberOfVoxels , BBox sceneBox)
{
	float3 dif = (position - sceneBox.min) + 0.0001f;
	int3 voxelPosition = convert_int3(dif * invWidth);
	int3 nVoxels = convert_int3(numberOfVoxels);
	voxelPosition = clamp(voxelPosition,(int3)(0,0,0) , nVoxels);
	return voxelPosition;
}

float3 voxelToPosition(BBox sceneBox, int3 position, float3 width)
{
	return sceneBox.min + (convert_float3(position) * width);
}

int findVoxelIndex(int3 position , float3 cellDimensions)
{
	return position.x + position.y * (int)cellDimensions.x + position.z * (int)cellDimensions.x * (int)cellDimensions.y;
}

int3 findVoxelPosition(int index , float3 cellDimensions)
{
	int3 position;
	position.z = index / ((int)(cellDimensions.x* cellDimensions.y));
	position.y = (index % ((int)(cellDimensions.x* cellDimensions.y))) / ((int)cellDimensions.x);
	position.x = index - position.y * cellDimensions.x - position.z *cellDimensions.x *cellDimensions.y;
	return position;
}

StepInfo findStepInfo(int3 currentCell,Ray ray, float minVal, 
	float maxVal,float3 voxelWidth,BBox box, float3 numberOfVoxels)
{
	
	float3 nextCrossing;
	float3 deltaT;
	int3 nextStep;
	int3 out;

	float3 voxelPositionPos =  voxelToPosition(box, currentCell + 1,voxelWidth.xyz);
	float3 voxelPositionNeg =  voxelToPosition(box, currentCell,voxelWidth.xyz);
	float3 intersectionPoint = ray.origin + ray.direction * minVal;

	if(ray.direction.x >= 0)
	{
		nextCrossing.x = minVal + (voxelPositionPos.x - intersectionPoint.x) / ray.direction.x;
		deltaT.x = voxelWidth.x/ ray.direction.x;
		nextStep.x = 1;
		out.x = numberOfVoxels.x;

	}
	else
	{
		nextCrossing.x = minVal + (voxelPositionNeg.x - intersectionPoint.x) / ray.direction.x;
		deltaT.x= -voxelWidth.x/ ray.direction.x;
		nextStep.x = -1;
		out.x = -1;
	}


	if(ray.direction.y >= 0)
	{
		nextCrossing.y = minVal + (voxelPositionPos.y - intersectionPoint.y) / ray.direction.y;
		deltaT.y = voxelWidth.y/ ray.direction.y;
		nextStep.y = 1;
		out.y = numberOfVoxels.y;

	}
	else
	{
		nextCrossing.y = minVal + (voxelPositionNeg.y- intersectionPoint.y) / ray.direction.y;
		deltaT.y= -voxelWidth.y/ ray.direction.y;
		nextStep.y = -1;
		out.y= -1;
	}

	if(ray.direction.z >= 0)
	{
		nextCrossing.z = minVal + (voxelPositionPos.z- intersectionPoint.z) / ray.direction.z;
		deltaT.z = voxelWidth.z/ ray.direction.z;
		nextStep.z = 1;
		out.z = numberOfVoxels.z;

	}
	else
	{
		nextCrossing.z = minVal + (voxelPositionNeg.z - intersectionPoint.z) / ray.direction.z;
		deltaT.z= -voxelWidth.z/ ray.direction.z;
		nextStep.z = -1;
		out.z = -1;
	}

	StepInfo s;
	s.nextCrossing= nextCrossing;
	s.deltaT = deltaT;
	s.nextStep = nextStep;
	s.out = out;

	return s;	
}

StepReturn stepThroughGrid(StepInfo s, int3 currentCell, float maxVal)
{

	bool run = true;

	int stepAxis =  (s.nextCrossing.x  < s.nextCrossing.y && s.nextCrossing.x < s.nextCrossing.z) ? 0 :(s.nextCrossing.y < s.nextCrossing.z) ? 1 : 2;



	if(stepAxis == 0)
	{


			if(maxVal < s.nextCrossing.x) 
			{
				run = false;
			}
			currentCell.x += s.nextStep.x;

	

			//if((ray.direction.x >= 0) ?  out.x <= currentCell.x : out.x >= currentCell.x)
			if( s.out.x == currentCell.x)
			{
				run = false;

			}
		
			s.nextCrossing.x += s.deltaT.x;

		
	
	}


	if(stepAxis == 1)
	{

			if(maxVal < s.nextCrossing.y)
			{
				run = false;
			}

			currentCell.y += s.nextStep.y;
			//if((ray.direction.y >= 0) ?  out.y <= currentCell.y : out.y >= currentCell.y)
			if( s.out.y == currentCell.y)
			{
				run = false;
			}
			s.nextCrossing.y += s.deltaT.y;

	}


	if(stepAxis == 2)
	{

			if(maxVal < s.nextCrossing.z)
			{
				run = false;
			}

			currentCell.z += s.nextStep.z;
			
			//if(out.z >= currentCell.z)
			if(s.out.z == currentCell.z)
			{
				run = false;
			}
			s.nextCrossing.z += s.deltaT.z;
		
	}


	StepReturn retS;
	retS.stepInfo = s;
	retS.continueStep = run;
	retS.currentCell = currentCell;

	return retS;
}

TriangleInfo triangleCollision(Ray ray, Triangle triangle)
{
	TriangleInfo tri;
	tri.v0 = triangle.v0;
	tri.v1 = triangle.v1;
	tri.v2 = triangle.v2;

	tri.hasIntersection = false;
	float3 V3 = tri.v2;
	float3 V1 = tri.v0;
	float3 V2 = tri.v1;



	float3 e1, e2;  //Edge1, Edge2
	float3 P, Q, T;
	float det, inv_det, u, v;
	float t;

	//Find vectors for two edges sharing V1
	e1=  V2 - V1;
	e2=  V3 - V1;
	//Begin calculating determinant - also used to calculate u parameter
	P = cross( ray.direction.xyz, e2);
	//if determinant is near zero, ray lies in plane of triangle
	det = dot(e1, P);
	//NOT CULLING
	if (det > -epsilion && det < epsilion) return tri;
	inv_det = 1.f / det;

	//calculate distance from V1 to ray origin
	T =  ray.origin.xyz - V1;

	//Calculate u parameter and test bound
	u = dot(T, P) * inv_det;
	//The intersection lies outside of the triangle
	if (u < 0.f || u > 1.f) return tri;

	//Prepare to test v parameter
	Q = cross( T, e1);

	//Calculate V parameter and test bound
	v = dot(ray.direction.xyz, Q) * inv_det;
	//The intersection lies outside of the triangle
	if (v < 0.f || u + v  > 1.f) return tri;

	t = dot(e2, Q) * inv_det;

	if (t > epsilion) { //ray intersection

		tri.hasIntersection = true;
		tri.distanceFromIntersection = t;

		return tri;
	}
	return tri;
}

bool insideBBox(float3 origin,float3 minPoint, float3 maxPoint)
{
	return (origin.x >= minPoint.x && origin.x <= maxPoint.x) &&
	(origin.y >= minPoint.y && origin.y <= maxPoint.y) &&
	(origin.z >= minPoint.z && origin.z <= maxPoint.z);
}

float surfaceArea(BBox box)
{
	float3 d = box.max - box.min;
	return 2.0f * (d.x * d.y + d.x * d.z + d.y * d.z);
}

float volumne(BBox box)
{
	float3 d = box.max - box.min;
	return d.x * d.y * d.z;
}

int maxExtent(BBox box)
{
	float3 d = box.max - box.min;
	return (d.x > d.y && d.x > d.z) ? 0 : (d.y > d.z) ? 1 : 2;
}

BBox createCellBox(int index, float3 numberOfVoxels, float3 delta, BBox box)
{
	BBox retBox;
	retBox.min = voxelToPosition(box,findVoxelPosition(index, numberOfVoxels),delta / numberOfVoxels );
	retBox.max = retBox.min  + (delta / numberOfVoxels);
	return retBox;
}

HitReturn hitBBox(Ray ray,float3 minPoint, float3 maxPoint)
{
	float a = 1.0 / ray.direction.x;
	float b = 1.0 / ray.direction.y;
	float c = 1.0 / ray.direction.z;

	float3 hitMin;
	float3 hitMax;

	if(a >= 0)
	{
		hitMin.x = (minPoint.x - ray.origin.x) * a; 
		hitMax.x = (maxPoint.x - ray.origin.x) * a; 
	}
	else
	{
		hitMin.x = (maxPoint.x - ray.origin.x) * a; 
		hitMax.x = (minPoint.x - ray.origin.x) * a; 
	}

	if(b >= 0)
	{
		hitMin.y = (minPoint.y - ray.origin.y) * b; 
		hitMax.y = (maxPoint.y - ray.origin.y) * b; 
	}
	else
	{
		hitMin.y = (maxPoint.y - ray.origin.y) * b; 
		hitMax.y = (minPoint.y - ray.origin.y) * b; 
	}

	if(c >= 0)
	{
		hitMin.z = (minPoint.z - ray.origin.z) * c; 
		hitMax.z = (maxPoint.z - ray.origin.z) * c; 
	}
	else
	{
		hitMin.z = (maxPoint.z - ray.origin.z) * c; 
		hitMax.z= (minPoint.z - ray.origin.z) * c; 
	}
	float minValue, maxValue;

	//largest entering value
	minValue = max(hitMin.x,hitMin.y);
	minValue = max(hitMin.z, minValue );

	// smallest exiting value
	maxValue = min(hitMax.x,hitMax.y);
	maxValue = min(hitMax.z, maxValue );

	HitReturn returnValue;
	returnValue.hit  =(minValue < maxValue && maxValue > epsilion);

	if(returnValue.hit)
	{
		returnValue.minValue = minValue;
		returnValue.maxValue = maxValue;
	}
	else
	{
		returnValue.minValue = 0.0f;
		returnValue.maxValue = 0.0f;	
	}


	return returnValue;
}
HitReturn hitBox(Ray ray, BBox box)
{
	return hitBBox(ray,box.min, box.max);
}


Ray generateRay(int2 pixelLocation, int width, int height, Camera camera, int2 dim, int sampleNumber , long seed)
{
	Ray ray;

	long xSeed = pixelLocation.x + dim.x + seed;
	long ySeed = pixelLocation.y + dim.y + seed * xSeed;
	float r1  = 2 * randFloat(xSeed) , dx = r1 < 1 ? sqrt(r1)-1 : 1 - sqrt(2 -r1);
	float r2  = 2 * randFloat(ySeed) , dy = r2 < 1 ? sqrt(r2)-1 : 1 - sqrt(2 -r2);


	//ray.direction = camera.u * (((dim.x + 0.5f +dx )/2.0f + pixelLocation.x)/width - 0.5f) + 
	//camera.v * (((dim.y + 0.5f +dy )/2.0f + pixelLocation.y)/height - 0.5f) + 
	//(camera.distance * camera.w);
	ray.origin.xyz =camera.position.xyz;
	float2 samplePoint = (float2)( (dim.x + r1) , (dim.y + r2));
	
	float3 direction;
	float3 origin = camera.position.xyz;
	float2 pp =  (float2)((pixelLocation.x - (0.5f * width)) + samplePoint.x/sampleNumber, (pixelLocation.y - (0.5 * height)) + samplePoint.y/sampleNumber);
	float2 pixelToRay = pp;


	ray.direction = normalize((pixelToRay.x * -camera.u) + (pixelToRay.y * -camera.v) - (camera.distance * camera.w));


	return ray;
}
SphereInfo sphereIntersection(Ray ray, float3 position, float radius)
{
	float3 originToShape = ray.origin.xyz - position;
	float a = dot(ray.direction.xyz, ray.direction.xyz);
	float b = 2.0f * dot(originToShape, ray.direction.xyz);
	float c = dot(originToShape, originToShape) - radius * radius;
	float disc = b*b - 4.0f * a * c;
	float t ;
	SphereInfo s;
	s.hasIntersection = false;
	if(disc > 0.0f)
	{
		float e = sqrt(disc);
		float denom = 2.0f * a;
		t = (-b-e)/denom;
		if(t > epsilion)	
		{
			s.normal = (float3) (((ray.origin.xyz - position) + t * ray.direction.xyz)/ radius);
			s.intersectionPoint = (ray.origin.xyz - position) + t * ray.direction.xyz;
			s.hasIntersection = true;
			s.distanceToIntersection = t;
			return s;
		}

		t = (-b + e) / denom;
		if(t > epsilion)
		{
			s.normal = (float3)(((ray.origin.xyz - position) + t * ray.direction.xyz)/radius);
			s.hasIntersection = true;
			s.intersectionPoint = (ray.origin.xyz - position) + t * ray.direction.xyz;
			s.distanceToIntersection = t;
			return s;
		}
	}

	return s;
}

bool bboxCollided(BBox b1 , BBox b2)
{
	return (b1.min.x <= b2.min.x + fabs(b2.min.x - b2.max.x) &&
		b2.min.x <= b1.min.x + fabs(b1.min.x - b1.max.x) &&

		b1.min.y <= b2.min.y + fabs(b2.min.y - b2.max.y) &&
		b2.min.y <= b1.min.y + fabs(b1.min.y - b1.max.y) &&

		b1.min.z <= b2.min.z + fabs(b2.min.z - b2.max.z) &&
		b2.min.z <= b1.min.z + fabs(b1.min.z - b1.max.z)) 
	;
}
bool bboxObjectCollided(BBox b1 , Mesh m, Object o1)
{

	BBox box;
	box.min = o1.box.min + m.position;
	box.max = o1.box.max + m.position;

	return bboxCollided(b1,box);

}

IntersectionInfo rayTraceKD(Ray ray,Camera camera, 
	__global Light * light, __global Object * objects,
	__global Triangle * triangles,
	 BBox box, __global Mesh * meshes, __global KDNode * kdNodes, __global int * nodeIndices
	, __local int * stack)
{
	int tail = 0;
	int head = 0;
	stack[head] = 0;
	int currentNode = 0;
	int previousPop = -1;
	BBox left;
	BBox right;
	HitReturn leftHit;
	HitReturn rightHit;
	IntersectionInfo intersect;
	intersect.distance = -1;
	float lastDistance = 10000000.0f;
	int leftIndex;
	int rightIndex;
	float3 normal;
	int objectColorIndex = -1;
	bool hit = false;

	while(head <= tail )
	{

		leftIndex = kdNodes[currentNode].nodes[0];
		rightIndex = kdNodes[currentNode].nodes[1];
		
		if(leftIndex != -1)
		{
			 left = kdNodes[leftIndex].boundingBox;
			 right = kdNodes[rightIndex].boundingBox;
			
			 leftHit = hitBBox(ray,left.min,left.max);
			 rightHit = hitBBox(ray,right.min,right.max);
		}

		if(leftIndex != -1 && leftHit.hit && previousPop != leftIndex && previousPop != rightIndex &&  kdNodes[leftIndex].numberOfObjects > 0 )//&& stack[tail] !=leftIndex) && stack[tail] !=rightIndex)
		{
			stack[++tail] = leftIndex;
			if(tail >= 0) currentNode = stack[tail];

			continue;
		}
		if(rightIndex != -1 && rightHit.hit && previousPop != rightIndex  &&  kdNodes[rightIndex].numberOfObjects > 0 )//&& stack[tail] !=leftIndex) && stack[tail] !=rightIndex)
		{
			stack[++tail] = rightIndex;
			if(tail >= 0) currentNode = stack[tail];
			continue;
		}
		
		
		if(kdNodes[currentNode].startingIndex != -1)
		{
			hit = false;
			int index = currentNode;
			for(int i = kdNodes[index].startingIndex; i< kdNodes[index].startingIndex + kdNodes[index].numberOfObjects; i++)
			{
				int objectIndex = nodeIndices[i];	
				TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
				triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
				if(triInfo.hasIntersection && lastDistance  > triInfo.distanceFromIntersection)
				{
					lastDistance = triInfo.distanceFromIntersection;
					normal = triInfo.normal;
					objectColorIndex = objectIndex;
					hit = true;
				}
			}
			/*
			for(int i = 0; i < 12 ; i++)
			{
				TriangleInfo triInfo = triangleCollision(ray,triangles[i]);
				triInfo.normal = triangles[i].normal;
				if(triInfo.hasIntersection && lastDistance  > triInfo.distanceFromIntersection)
				{
					lastDistance = triInfo.distanceFromIntersection;
					normal = triInfo.normal;
					objectColorIndex = i;
					hit = true;
				}
			}*/

			if(hit)
			{
				intersect.direction = ray.direction;
				intersect.position = ray.direction * lastDistance + ray.origin;
				intersect.distance = lastDistance;
				intersect.objectIndex = objectColorIndex;
				intersect.type = TRIANGLE;
			}
		}
		previousPop = currentNode;
		tail--;
		if(tail >= 0) currentNode = stack[tail];
		
		//if(tail >= 0) currentNode = stack[tail];

	}


	return intersect;
}

IntersectionInfoO rayTraceO(Ray ray,Camera camera, 
	__global Light * light, __global Object * objects, __global Octree * octreeNodes,
	__global Triangle * triangles,
	 BBox box, __global Mesh * meshes, 
	 __local int * depthTrack , __local float * minDepth,
	 int maxDepth, int originNode)
{
	IntersectionInfoO intersect;
	intersect.distance = -1;

    HitReturn hitRet = hitBox( ray,  octreeNodes[0].boundingBox);
    int cheapestDepth = -1;
    int leafNode = -1;
    bool continueWhile = true;
    float minValue = 1000000.0f;
	int node = 0;

    if(hitRet.hit)
    {
		//int startingIndex = 1;//octreeNodes[node].childrenStart;

    	while(continueWhile)
    	{
			int startingIndex = octreeNodes[node].childrenStart;

			bool hitNode = false;

			//do you have any children 
			if(octreeNodes[node].childrenStart != -1)
			{
				//if(startingIndex > 73)break;
				for(int i = startingIndex;i < startingIndex + 8; i++)
				{
					if(octreeNodes[i].numberOfObjects > 0)
					{
						hitRet = hitBox( ray,  octreeNodes[i].boundingBox); 
						if(hitRet.minValue < minValue)
					{
							node = i;
							hitNode = true;
						}   
					}	
				}

				//0, 8 ,  24(-1)
				//is it a child
				if(!hitNode)
				{
					continueWhile = false;
					break;
				}

				if( octreeNodes[node].childrenStart < 0)
				{
					leafNode = node;
					continueWhile = false;
					break;
				}
			}	
			else
			{
				continueWhile = false;
			}
    	}

		bool hasLeaf = leafNode != -1;
		
		if(hasLeaf)
		{
			float lastDistance = 1000000.0f;
			float3 normal;
			int objectColorIndex;

			for(int i = 0 ; i < octreeNodes[leafNode].numberOfObjects; i ++)
			{		
				int objectIndex = octreeNodes[leafNode].objectIndices[i];	
				TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
				triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
				if(triInfo.hasIntersection && lastDistance  > triInfo.distanceFromIntersection)
				{
						//outColor = (uint4)adsLight(objects[objectIndex], light[0], triInfo);
					lastDistance = triInfo.distanceFromIntersection;
					normal = -triInfo.normal;
					objectColorIndex = objectIndex;
					break;
				}
			}

			intersect.direction = ray.direction;
			intersect.position = ray.direction * lastDistance + ray.origin;
			intersect.distance = lastDistance;
			intersect.objectIndex = objectColorIndex;
			intersect.type = TRIANGLE;
			intersect.octreeNode = leafNode;
			intersect.endDepth = cheapestDepth;
		}

	}

	return intersect;
}

IntersectionInfo rayTrace(Ray ray,Camera camera, 
	__global int * cellIndices, __global int * objectIndices,
	__global Light * light, __global Object * objects,
	__global Triangle * triangles, int cellIndex, int3 currentCell,
	 BBox box,float3 voxelWidth,  float3 cellDimensions, __global Mesh * meshes)
{
	bool run = true;
	IntersectionInfo intersect;
	intersect.distance = -1;
	float lastDistance = 100000.0f;
	HitReturn hitCheck = hitBBox(ray,box.min,box.max);
	float maxVal=hitCheck.maxValue;
	StepInfo s=  findStepInfo(currentCell, ray,hitCheck.minValue, hitCheck.maxValue, voxelWidth, box, cellDimensions );

	while(run)
	{
		int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];
		int objectColorIndex = 0;
		float3 normal;
		for(int i = 0; i <  cellObjectNumber; i++ )
		{
			int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;
			if(meshes[objects[objectIndex].meshIndex].position.w == 0.0)
			{
				TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
				triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
				if(triInfo.hasIntersection && lastDistance  > triInfo.distanceFromIntersection)
				{
					//outColor = (uint4)adsLight(objects[objectIndex], light[0], triInfo);
					lastDistance = triInfo.distanceFromIntersection;
					normal = triInfo.normal;
					objectColorIndex = objectIndex;
					run = false;
				}
			}
		}
		if(!run)
		{
			intersect.direction = ray.direction;
			intersect.position = ray.direction * lastDistance + ray.origin;
			intersect.distance = lastDistance;
			intersect.objectIndex = objectColorIndex;
			intersect.type = TRIANGLE;
		}
		if(run)
		{
			StepReturn stepReturned =  stepThroughGrid(s,currentCell,maxVal);
			currentCell = stepReturned.currentCell;
			run = stepReturned.continueStep;
			s = stepReturned.stepInfo;
		}
		cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
	}
	return intersect;
}