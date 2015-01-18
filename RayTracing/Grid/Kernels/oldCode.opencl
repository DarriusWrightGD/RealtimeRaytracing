
	//float3 s = dot(normal, lightVector) * normal - lightVector;
	//float3 r =  (2.0f*(dot(normal, lightVector) * normal)) - lightVector;
	//float3 v = intersectionPoint - camera.position;
	//r = normalize(r);
	//v = normalize(v);
	//s = normalize(s);

	//if(dot(r,v) < 0)
	//{
	//	r = (float3)(0.0f,0.0f,0.0f);
	//	v = (float3)(0.0f,0.0f,0.0f);
	//}
	//float specularPower = pow(dot(v,r), material.specular.w);

	//float3 specular = light.material.specular.xyz * specularPower * material.specular;// * attenuation;//* (light.type == POINT_LIGHT) ? attenuation : 1.0f;
	//if(light.type == POINT_LIGHT)
	//{
	//	float attenuation = clamp((1.0f - length(lightVector)/ light.direction.w) , 0.0f, 1.0f);
	//	diffuse *= attenuation;
	//	specular *= attenuation;
	//}


	

/*
__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, __write_only __global float * depthBuffer,
	sampler_t sampler, int width, int height, __global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth, int seed, int maxDepth,__local float3 * emissions, __local float3 * levelColor, __local int * objectIndex,
	 int shadowsEnabled, int reflectionsEnabled, int refractionsEnabled)

{
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	uint4 outColor = (uint4)(0,0,0,255);
	float4 color;
	color.xyz = (float3)(0,0,0);
	color.w = 1.0f;

	//write_imagef(depthBuffer,outImageCoord,0.0f);
	float inverseSamples = 1.0f ;
	for(int sy = 0; sy < 2; sy++ )
	{
		for(int sx = 0; sx < 2; sx++ )
		{
			float3 radiantColor = (float3)(0,0,0);
			int sam = 1;
			for(int sam = 0; sam < 1; sam++)
			{
				Ray ray= generateRay(outImageCoord, width, height,camera,(int2)(sx,sy), samplesSquared, seed);
				//radiantColor = (float3)(1,1,1);
				radiantColor = radiance(ray,seed + outImageCoord.x + outImageCoord.y + sam + sx + sy, maxDepth,   width,  height, objects, triangles, light, box,cells, cellDimensions,
				cellIndices, objectIndices, camera, samples, samplesSquared , delta, deltaInv, voxelInvWidth, numberOfVoxels , voxelWidth, levelColor, objectIndex , emissions).xyz * inverseSamples;
			}

			color.xyz += clamp(radiantColor, (float3)(0,0,0), (float3)(1,1,1)) * 0.25f;
		}
	}


	outColor = toRGBA(color);
	write_imageui(dstImage, outImageCoord, outColor);

}
*/
/*
__kernel void drawScene(__read_only image2d_t srcImg, __write_only image2d_t dstImage, __write_only __global float * depthBuffer,
	sampler_t sampler, int width, int height, __global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, BBox box,
	__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth, int seed, int shadowsEnabled, int reflectionsEnabled, int refractionsEnabled)

{
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	uint4 outColor = (uint4)(0,0,0,255);
	//write_imagef(depthBuffer,outImageCoord,0.0f);


		for(int row = 0; row < samplesSquared; row++)
		{
			for(int column = 0; column < samplesSquared; column++ )
			{
				int depthBufferSampleLocation = ((outImageCoord.y * width * samples) + (outImageCoord.x * samples)  ) + ((row * samplesSquared) + column);
				depthBuffer[ depthBufferSampleLocation] =  0.0f;
				int2 dim = (int2)(row,column);
				//int2 dim = (int2)(0,0);
				Ray ray = generateRay(outImageCoord, width, height, camera,dim,samples, seed);
				HitReturn hitCheck = hitBBox(ray,box.min,box.max);

				if(hitCheck.hit)
				{
					int3 currentCell;
					float minVal = 0.0f;
					float maxVal = 0.0f;
					float lastDistance = 10000000.0f;

					
					if(!insideBBox(ray.origin.xyz,box.min,box.max ))
					{
						float3 p = ray.origin.xyz + (hitCheck.minValue * ray.direction.xyz);
						minVal = hitCheck.minValue;
						maxVal = hitCheck.maxValue;
					    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
					}
					else
					{
						//currentCell = positionToVoxel(ray.origin, voxelInvWidth, numberOfVoxels-1,box);
						currentCell = (int3)(convert_int3(clamp((ray.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
						maxVal = hitCheck.maxValue;

					}					

					int cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;

					StepInfo s = findStepInfo(currentCell,ray,minVal, maxVal, voxelWidth, box, numberOfVoxels );
					
					bool run = true;

					while(run)
					{
						
						int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];
						int objectColorIndex = 0;
						float3 normal;
						for(int i = 0; i <  cellObjectNumber; i++ )
						{
							int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;
							if(objects[objectIndex].position.w == 0.0)
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
							uint4 lightColor = adsLight(light[0], objects[objectColorIndex].material, camera, ray.direction * lastDistance, normal);
							outColor +=  lightColor;
							depthBuffer[depthBufferSampleLocation] = lastDistance;
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
			}
		}
	}
	outColor.xyz/= samples;
	outColor.w = 255;
	write_imageui(dstImage, outImageCoord, outColor);

}
*/


__kernel void drawShadowRays(__write_only image2d_t dstImage, 
	__global float * depthBuffer,sampler_t sampler, int width, int height, 
	__global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, 
	BBox box,__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth , int seed)
{
/*
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));


		bool shadowLocated = false;
		uint4 outColor;// = (uint4)(0,0,0,255);

		for(int row = 0; row < samplesSquared; row++)
		{
			for(int column = 0; column < samplesSquared; column++ )
			{
				int depthBufferSampleLocation = ((outImageCoord.y * width * samples) + (outImageCoord.x * samples)  ) + ((row * samplesSquared) + column);

				float lastDistance = depthBuffer[depthBufferSampleLocation];

				if(lastDistance> 0.0f)
				{

				int2 dim = (int2)(row,column);
				//int2 dim = (int2)(0,0);
				
				
				Ray ray = generateRay(outImageCoord, width, height, camera,dim,samples , seed );
				HitReturn hitCheck = hitBBox(ray,box.min,box.max);
				bool shadowRun = true;											
				 
				Ray shadowRay;
				shadowRay.origin = (ray.direction.xyz * lastDistance) + ray.origin.xyz;
				shadowRay.direction = normalize(light[0].position - shadowRay.origin);
				shadowRay.origin += (shadowRay.direction * epsilion);
				HitReturn shadowHit = hitBBox(shadowRay,box.min,box.max);
				//int3 currentCell = (int3)(convert_int3(clamp((shadowRay.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
					int3 currentCell;
					float minVal = 0.0f;
					float maxVal = 0.0f;
					
					if(!insideBBox(shadowRay.origin.xyz,box.min,box.max ))
					{
						float3 p = shadowRay.origin.xyz + (hitCheck.minValue * shadowRay.direction.xyz);
						minVal = hitCheck.minValue;
						maxVal = hitCheck.maxValue;
					    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
					}
					else
					{
						//currentCell = positionToVoxel(ray.origin, voxelInvWidth, numberOfVoxels-1,box);
						currentCell = (int3)(convert_int3(clamp((shadowRay.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
						maxVal = hitCheck.maxValue;

					}	

				StepInfo shadowStep = findStepInfo(currentCell,shadowRay,0.0f, shadowHit.maxValue, voxelWidth, box, numberOfVoxels );

				StepReturn stepRet =  stepThroughGrid(shadowStep,currentCell,shadowHit.maxValue);
				currentCell = stepRet.currentCell;
				shadowStep = stepRet.stepInfo;
				shadowRun = stepRet.continueStep;
				
				int cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
				
				while(shadowRun)
				{
					int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];

					for(int i = 0; i <  cellObjectNumber && shadowRun; i++ )
					{
						int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;

						TriangleInfo triInfo = triangleCollision(shadowRay,triangles[objects[objectIndex].triangleIndex]);
						triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
						if(triInfo.hasIntersection)
						{
							outColor = (uint4)(0,0,0,255);
				//write_imageui(dstImage, outImageCoord, outColor);
							
							shadowRun = false;
							shadowLocated = true;
						}
							
					}
					if(shadowRun)
					{
						StepReturn stepReturned =  stepThroughGrid(shadowStep,currentCell,shadowHit.maxValue);
						currentCell = stepReturned.currentCell;
						shadowRun = stepReturned.continueStep;
						shadowStep = stepReturned.stepInfo;
					}

					cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
				
				}
			}
		}


		if(shadowLocated)
		{
			outColor.xyz/=samples;
			write_imageui(dstImage, outImageCoord, outColor);
		}
	}*/
}



__kernel void drawReflectionRays(__write_only image2d_t dstImage, 
	__global float * depthBuffer,sampler_t sampler, int width, int height, 
	__global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, 
	BBox box,__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth, int numberOfReflections, int seed)
{
	/*
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
	uint4 outColor = (uint4)(0,0,0,255);
	bool reflectionLocated = false;


		for(int row = 0; row < samplesSquared; row++)
		{

			for(int column = 0; column < samplesSquared; column++ )
			{
				int depthBufferSampleLocation = ((outImageCoord.y * width * samples) + (outImageCoord.x * samples)  ) + ((row * samplesSquared) + column);
				float lastDistance = depthBuffer[depthBufferSampleLocation];
				Ray ray = generateRay(outImageCoord, width, height, camera,(int2)(row,column),samples , seed);


				for(int i = 0; i < numberOfReflections; i++)
				{ 
					bool reflectedOnObject = false;

				if(lastDistance> 0.0f)
				{



				HitReturn hitCheck = hitBBox(ray,box.min,box.max);

				int3 currentCell;
				
				if(!insideBBox(ray.origin.xyz,box.min,box.max ))
				{
					float3 p = ray.origin.xyz + (hitCheck.minValue * ray.direction.xyz);
				    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
				}
				else
				{
					currentCell = (int3)(convert_int3(clamp((ray.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
				}		

				int cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
				float3 normal = (float3)(0,0,0);

				int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];
				bool findTri = false;
				for(int i = 0; i <  cellObjectNumber && !findTri; i++ )
				{
						int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;

						TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
						triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
						if(triInfo.hasIntersection)
						{
							outColor = adsLight(light[0], objects[objectIndex].material, camera, ray.direction * triInfo.distanceFromIntersection, triInfo.normal);
							normal = triInfo.normal;
							findTri = true;
						}			
				}
				outColor.w = 255;
				bool reflectionRun = true;											
				float3 reflected =-ray.direction  - 2 * dot(ray.direction , normal) * normal;
				Ray reflectionRay;
				reflectionRay.direction = reflected;
				reflectionRay.origin = ((ray.direction.xyz * lastDistance) + ray.origin.xyz)+ (reflected * epsilion);

					HitReturn reflectionHit = hitBBox(reflectionRay,box.min,box.max);
					currentCell = (int3)(convert_int3(clamp((reflectionRay.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));


					StepInfo reflectionStep = findStepInfo(currentCell,reflectionRay,0.0f, reflectionHit.maxValue, voxelWidth, box, numberOfVoxels );
								//StepInfo s = findStepInfo(currentCell,ray,minVal, maxVal, voxelWidth, box, numberOfVoxels );

					StepReturn stepRet =  stepThroughGrid(reflectionStep,currentCell,reflectionHit.maxValue);
					currentCell = stepRet.currentCell;
					reflectionStep = stepRet.stepInfo;
					reflectionRun = stepRet.continueStep;
					
					cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
					int reflectedNumber = 0;
					while(reflectionRun)
					{
						int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];

						for(int i = 0; i <  cellObjectNumber && reflectionRun; i++ )
						{
							int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;

							TriangleInfo triInfo = triangleCollision(reflectionRay,triangles[objects[objectIndex].triangleIndex]);
							triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
							if(triInfo.hasIntersection)
							{
								outColor.xyz += mixUIntColor(outColor.xyz,adsLight(light[0], objects[objectIndex].material, camera, reflectionRay.direction * triInfo.distanceFromIntersection, triInfo.normal).xyz);//convert_uint3(finalColor);//adsLightT(objects[objectIndex], light[0], triInfo).xyz;
								//clamp(outColor, (uint4)(0,0,0,0), (uint4)(255,255,255,255));
								//outColor = adsLight(light[0], objects[objectIndex].material, camera, reflectionRay.direction * triInfo.distanceFromIntersection, triInfo.normal);
								
								lastDistance = triInfo.distanceFromIntersection;
								reflectionRun = false;
								reflectionLocated = true;
								reflectedOnObject = true;
							}

								
						}



						if(reflectionRun)
						{
							StepReturn stepReturned =  stepThroughGrid(reflectionStep,currentCell,reflectionHit.maxValue);
							currentCell = stepReturned.currentCell;
							reflectionRun = stepReturned.continueStep;
							reflectionStep = stepReturned.stepInfo;
						}

						cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
						
					}
					if(reflectedOnObject)
					{
						float3 reflected =-reflectionRay.direction  - 2 * dot(reflectionRay.direction , normal) * normal;
						ray.origin = (lastDistance * reflectionRay.direction) + reflectionRay.origin;
						ray.direction = reflected;
						ray.origin +=  (reflected * epsilion);
					}
					//redo ray stuff
				}
			}
		}
	}
	if(reflectionLocated)
	{
		outColor.xyz/= samples;
	//	outColor.xyz/= numberOfReflections;
		write_imageui(dstImage, outImageCoord, outColor);
	}
	*/
}

__kernel void drawRefractionRays(__write_only image2d_t dstImage, 
	__global float * depthBuffer,sampler_t sampler, int width, int height, 
	__global Object * objects, __global Triangle * triangles, 
	__global Light * light, int numberOfObjects, int numberOfLights, 
	BBox box,__global int * cells, float3 cellDimensions,
	__global int * cellIndices, __global int * objectIndices,
	Camera camera, int samples, int samplesSquared , 
	float3 delta, float3 deltaInv, float3 voxelInvWidth, float3 numberOfVoxels 
	,float3 voxelWidth, int numberOfReflections, int seed )
{
/*
	int2 outImageCoord = (int2)(get_global_id(0),get_global_id(1));
				uint4 outColor = (uint4)(0,0,0,255);
		bool refractionLocated  = false;
		for(int row = 0; row < samplesSquared; row++)
		{
			for(int column = 0; column < samplesSquared; column++ )
			{

				int depthBufferSampleLocation = ((outImageCoord.y * width * samples) + (outImageCoord.x * samples)  ) + ((row * samplesSquared) + column);
				float lastDistance = depthBuffer[depthBufferSampleLocation];

				if(lastDistance> 0.0f)
				{


				Ray ray = generateRay(outImageCoord, width, height, camera,(int2)(row,column),samples , seed);

				HitReturn hitCheck = hitBBox(ray,box.min,box.max);

				int3 currentCell;
				
				if(!insideBBox(ray.origin.xyz,box.min,box.max ))
				{
					float3 p = ray.origin.xyz + (hitCheck.minValue * ray.direction.xyz);
				    currentCell = (int3)(convert_int3(clamp((p - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), cellDimensions - 1.0f)));
				}
				else
				{
					currentCell = (int3)(convert_int3(clamp((ray.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));
				}		

				int cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
				float3 normal = (float3)(0,0,0);

				int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];
				bool findTri = false;
				int objectIndex = 0;
				float oDistance = 0.0f;
				for(int i = 0; i <  cellObjectNumber && !findTri; i++ )
				{
						objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;

						TriangleInfo triInfo = triangleCollision(ray,triangles[objects[objectIndex].triangleIndex]);
						triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
						if(triInfo.hasIntersection)
						{
							outColor = adsLight(light[0], objects[objectIndex].material, camera, ray.direction * triInfo.distanceFromIntersection, triInfo.normal);
							normal = triInfo.normal;
							oDistance = triInfo.distanceFromIntersection;
							findTri = true;
						}			
				}
				//outColor.w = 255;
				bool refractionRun = true;											
				float3 lightDirection = (light[0].type == DIRECTIONAL_LIGHT) ? light[0].direction.xyz : light[0].position.xyz - (ray.direction.xyz * oDistance);
				float angle  = -dot(-ray.direction, normal);
				float n1 = airIndex;
				float n2 = 1.46f;//objects[objectIndex].material.refraction;

				float index = n1/n2;

				float refractedAngle = sqrt(1- (index*index) * (1 - (angle * angle)));
				//float refractedAngle = (index*index) * (1 - (angle * angle));
				if(refractedAngle>= 0.0f && findTri)
				{

					float3 refracted = (index * (angle - refractedAngle))  * - normal - (index * -ray.direction);
					Ray refractionRay;
					refractionRay.direction = normalize(refracted);

					refractionRay.origin = ((ray.direction.xyz * lastDistance) + ray.origin.xyz) + (refracted * epsilion);

					HitReturn refractionHit = hitBBox(refractionRay,box.min,box.max);
					currentCell = (int3)(convert_int3(clamp((refractionRay.origin.xyz - box.min) * cellDimensions/ (box.max- box.min),(float3)(0,0,0), convert_float3(cellDimensions) - 1.0f)));


					StepInfo refractionStep = findStepInfo(currentCell,refractionRay,0.0f, refractionHit.maxValue, voxelWidth, box, numberOfVoxels );
				
					StepReturn stepRet =  stepThroughGrid(refractionStep,currentCell,refractionHit.maxValue);
					currentCell = stepRet.currentCell;
					refractionStep = stepRet.stepInfo;
					refractionRun = stepRet.continueStep;
							
					cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
				
					while(refractionRun)
					{
						int cellObjectNumber = (cellIndex > 0) ? cellIndices[cellIndex]- cellIndices[cellIndex-1]  : cellIndices[0];

						for(int i = 0; i <  cellObjectNumber && refractionRun; i++ )
						{
							int objectIndex = objectIndices[cellIndices[ (cellIndex > 0) ? cellIndex-1 : 0] + i]-1;

							TriangleInfo triInfo = triangleCollision(refractionRay,triangles[objects[objectIndex].triangleIndex]);
							triInfo.normal = triangles[objects[objectIndex].triangleIndex].normal;
							if(triInfo.hasIntersection)
							{
								///float frenselEffect = mix(pow(1 - dot(ray.direction.xyz,normal), 3) ,1,0.1);
								//outColor.xyz = mixUIntColor(convert_uint3(convert_float3(outColor.xyz) * (1 - frenselEffect) * 0.5f), convertColor(objects[objectIndex].material.diffuse)); 
								outColor.xyz = mixUIntColor(outColor.xyz,adsLight(light[0], objects[objectIndex].material, camera, refractionRay.direction * triInfo.distanceFromIntersection, triInfo.normal).xyz);//convert_uint3(finalColor);//adsLightT(objects[objectIndex], light[0], triInfo).xyz;
								refractionRun = false;//(numberOfIter < 2);
								refractionLocated = true;
							}
								
						}
						if(refractionRun)
						{
							StepReturn stepReturned =  stepThroughGrid(refractionStep,currentCell,refractionHit.maxValue);
							currentCell = stepReturned.currentCell;
							refractionRun = stepReturned.continueStep;
							refractionStep = stepReturned.stepInfo;
						}

						cellIndex = currentCell.x + currentCell.y * cellDimensions.x + currentCell.z * cellDimensions.x * cellDimensions.y;
						
					}
				}
			}
		}
		if(refractionLocated)
		{
			outColor.xyz/= samples;
			write_imageui(dstImage, outImageCoord, outColor);
		}

	}
	*/
}

uint4 adsLight(Light light, Material material, Camera camera, float3 intersectionPoint,  float3 normal)
{
	float3 lightVector ;//=normalize(light.position - intersectionPoint);// normalize( light.position.xyz - object.position.xyz);
	lightVector = (light.type == DIRECTIONAL_LIGHT) ? normalize(light.direction.xyz) : normalize(light.position - intersectionPoint);
	float lightDotNormal = max(dot(lightVector.xyz,normal.xyz), 0.0f);
	float diffuseFactor = dot(normal,lightVector);
	float3 diffuse ;//= light.material.diffuse.xyz * max(diffuseFactor, 0.0f) * material.diffuse.xyz;// * (light.type == POINT_LIGHT) ? attenuation : 1.0f;
	float3 ambient ;//= material.ambient.xyz * light.material.ambient.xyz;

	
	float3 finalColor =   ambient + diffuse;// + specular;
	
	return (uint4)((convert_uint3(finalColor * 255.0f)),255);	
}

