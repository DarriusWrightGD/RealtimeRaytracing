float randFloat(long seed)
{
	//Java's implementation
	long randomNumber = ((seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1)) ;
	long randomNumber2 = (((seed + seed) * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1)) ;
	
	long minNum = min(randomNumber, randomNumber2);
	long maxNum = max(randomNumber, randomNumber2);


	float result =  (float)minNum/ (float)maxNum;//nextPowerOfTwo(randomNumber) ;
	return result;
}

//////////////////////////////////
//OpenGL Development Cookbook
//////////////////////////////////
float random(float3 scale, float seed,float3 imageCoord) {	
	float randomNum =sin(dot(imageCoord.xyz + seed, scale)) * 43758.5453f + seed; 
	return randomNum - (float)((int)randomNum);	
}	
//gives a uniform random direction vector
float3 uniformlyRandomDirection(float seed,float3 imageCoord) {		
	float u = random((float3)(12.9898, 78.233, 151.7182), seed, imageCoord);		
	float v = random((float3)(63.7264, 10.873, 623.6736), seed, imageCoord);		
	float z = 1.0 - 2.0 * u;		
	float r = sqrt(1.0 - z * z);	
	float angle = 6.283185307179586 * v;	
	return (float3)(r * cos(angle), r * sin(angle), z);	
}	
//returns a uniformly random vector
float3 uniformlyRandomVector(float seed, float3 imageCoord) 
{		
	return uniformlyRandomDirection(seed, imageCoord) *  (random((float3)(36.7539, 50.3658, 306.2759), seed,imageCoord));	
}
///////////////////////////////