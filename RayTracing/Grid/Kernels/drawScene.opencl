float3 pathTrace(Ray ray,__global Light * light,float t , float4 backgroundColor,int MAX_BOUNCES, BBox box, float seed,  int width, 
	int height, __global Object * objects, __global Triangle * triangles, 
	__global int * cells, float3 cellDimensions,__global int * cellIndices, __global int * objectIndices,
	Camera camera, float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels ,
	float3 voxelWidth , float3 imageCoord , 
	float3 lightVector,int shadowsEnabled , 
	__global Mesh * meshes, __global KDNode * kdNodes,
	__global int * kdIndices,__local int * stack)// __global Octree * octreeNodes,__local int * depthTrack, __local float *  minDepth, int maxDepth)
{
	float3 mask = (float3)(1,1,1);
	float3 colorSum = (float3)(0,0,0);
	float3 surfaceColor = backgroundColor.xyz;

	float diffuse = 1;
	float color = 1;
	int bounce = 0;
	for(; bounce < MAX_BOUNCES; bounce++)
	{
		HitReturn hitRet = hitBox(ray,box);

		if(hitRet.minValue > hitRet.maxValue)
			continue;
		if(hitRet.maxValue < t)
			t = hitRet.maxValue + 1;

		
		//float4 val = (float4)(t,0,0,0); 

		int cellIndex;
		float minVal = 0.0f;
		float maxVal = 0.0f;
		int3 currentCell;
		
		if(!insideBBox(ray.origin.xyz,box.min,box.max ))
		{
			float3 p = ray.origin.xyz + (hitRet.minValue * ray.direction.xyz);
			minVal = hitRet.minValue;
			maxVal = hitRet.maxValue;
		    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
		}
		else
		{
			currentCell = (int3)(convert_int3(clamp((ray.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
			maxVal = hitRet.maxValue;

		}	
		cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
		
		float3 normal;

		//find the inttersection in the scene
		IntersectionInfo intersectO  = rayTrace(ray, camera,cellIndices, objectIndices,light,
				objects, triangles, cellIndex, currentCell, box,voxelWidth,numberOfVoxels, meshes);
		
		//IntersectionInfoO intersectO = rayTraceO(ray, camera, light,
		//	objects,octreeNodes, triangles, box, meshes, depthTrack,minDepth,maxDepth,0);

		//IntersectionInfo intersectO = rayTraceKD(ray, camera, light,
		//	objects, triangles, box, meshes,kdNodes, kdIndices,stack);


		if(intersectO.distance == -1) break;

		normal = triangles[objects[intersectO.objectIndex].triangleIndex].normal;
		Material material = meshes[objects[intersectO.objectIndex].meshIndex].material;

		if(intersectO.distance < t)
		{
			surfaceColor = material.color.xyz;
			
			float3 hit = intersectO.position;
			ray.origin = hit;
			float3 orientedNormal = dot(normal, ray.direction) < 0? normal : -normal;
			float3 direction = ray.direction;
			ray.direction = uniformlyRandomDirection(seed + (float)(bounce), imageCoord);
			float3 jitteredLight = lightVector + ray.direction;
			float3 L = normalize(jitteredLight - hit);
			mask *= surfaceColor;

			if(material.type == DIFFUSE)
			{
				color = max(0.0f, dot(L,orientedNormal));
				float shadowMult = 1.0f;
				IntersectionInfo shadowIntersect;
				shadowIntersect.distance = -1.0f;
				if(shadowsEnabled)
				{
					Ray shadowRay;
					shadowRay.direction = L;
					shadowRay.origin = hit;
	
					shadowIntersect  = rayTrace(shadowRay, camera,cellIndices, objectIndices,light,
					objects, triangles, cellIndex, currentCell, box,voxelWidth,numberOfVoxels, meshes);
				}
	
				shadowMult = shadowIntersect.distance == -1.0f ? 1.0f : 0.5f;
	
				colorSum  += mask * color * shadowMult;
			}
			else
			{
				float3 reflected = reflect(direction, orientedNormal);
				if (material.type == SPECULAR)
				{
					
					color *= dot(reflected,orientedNormal);
					ray.direction = reflected;
					colorSum  += mask * color;
				}
				else
				{
					bool into = dot(normal , orientedNormal) > 0;
					float nc = 1.0f;
					float nt = material.refraction;
					float nnt = into ? nc / nt : nt/nc;
					float ddn = dot(direction, orientedNormal);
					float cos2t = 1- nnt * nnt * (1-ddn * ddn);

					if(cos2t < 0)
					{
						color *= dot(reflected,orientedNormal);
						ray.direction = reflected;
						colorSum  += mask * color;
					}
					else
					{
						float3 tDir = normalize(direction * nnt - normal * ((into ? 1 : -1)* (ddn * nnt + sqrt(cos2t))));
						ray.direction = tDir;
						float a = nt - nc, b = nt + nc, R0 = a* a/ (b*b), c = 1-(into?-ddn : dot(tDir,normal));
						float Re = R0 + (1- R0) *c*c*c*c*c, Tr = 1- Re, P= .25 + .5 * Re, RP = Re /P , TP = Tr /(1-P);
						float randTrans = random((float3)(12.9898, 78.233, 151.7182),seed + bounce, imageCoord);
						
						color *= (bounce > 2) ? ((randTrans < P) ? RP : TP) : Re  + Tr;
						colorSum  += mask * color;

					}

				}
			}


			
			t = intersectO.distance;

		}

	}

	return (colorSum == (float3)(0,0,0)) ? surfaceColor * diffuse : colorSum / (float)(bounce);
}

__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, __write_only __global float * depthBuffer,
	sampler_t sampler, int width, int height, __global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int numberOfSamples , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth, float seed, int MAX_BOUNCES,
	 int shadowsEnabled, int reflectionsEnabled, int refractionsEnabled,
	 float4 backgroundColor, __global Mesh * meshes , __global KDNode * nodes,
	  __global int * kdIndices, __local int * stack)// __global Octree * octreeNodes, __local int * depthTrack, __local float * minDepth, int maxDepth )
{
	uint4 outColor;
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	float3 imageCoord = (float3)(get_global_id(0),get_global_id(1),1.0f);
	float4 color = backgroundColor;
	float4 radiantColor = (float4)(0,0,0,0);
	float inverseSamples = 1.0f/ numberOfSamples ;

	int unitSamples = 1;
	float divideFactor = 1/ (float)(unitSamples * unitSamples);
	for(int sy = 0; sy < unitSamples; sy++ )
	{
		for(int sx = 0; sx < unitSamples; sx++ )
		{
			float3 radiantColor = (float3)(0,0,0);
			
			for(int sam = 0; sam < numberOfSamples; sam++)
			{
				Ray ray= generateRay(outImageCoord, width, height,camera,(int2)(sx,sy), numberOfSamples, seed);
				HitReturn hitRet = hitBox(ray,box);
				if(hitRet.hit)
				{
					float3 lightVector = light[0].position + uniformlyRandomVector(seed , imageCoord);
					radiantColor += (float4)(pathTrace(ray,light,hitRet.maxValue, backgroundColor,MAX_BOUNCES, box,  seed,   width, 
	 										height, objects, triangles, cells, cellDimensions,cellIndices, objectIndices,camera,  delta,  
											 deltaInv, voxelInvWidth,  numberOfVoxels , voxelWidth, imageCoord, lightVector, shadowsEnabled, 
											 meshes, nodes, kdIndices, stack),0.0f)* inverseSamples;//octreeNodes, depthTrack, minDepth, maxDepth) ,0.0f)* inverseSamples;
				}
			}
			color.xyz += clamp(radiantColor.xyz, (float3)(0,0,0), (float3)(1,1,1)) * divideFactor;
		}
	}


	outColor = toRGBA( color);
	write_imageui(dstImage, outImageCoord, outColor);

}
