__constant float epsilion = 0.00001;

typedef struct
{
	float3 min;
	float3 max;
}BBox;

typedef struct 
{
	float3 ambient;
	float3 diffuse;
	float3 specular;


}Material;

typedef struct 
{

	uint code;
	uint index;

}MortonCode;



typedef struct
{
    Material material;
    BBox box;
    float4 position;
    int2 triangleIndices;
    int cellId;
    uint code;
    
    
}Object;


typedef struct 
{
	unsigned int code;
	unsigned int index;
}MortonNode;


typedef struct
{
	float3 u;
	float3 v;
	float3 w;
	float3 up;
	float3 lookAt;
	float3 position;
	float distance;
}Camera;

typedef struct
{
	float3 direction;
	float3 origin;
}Ray;

typedef struct
{
	bool hasIntersection;
	float distanceToIntersection;
	float3 intersectionPoint;
	float3 normal;
}SphereInfo;

typedef struct
{
	Material material;
	float3 position;
}Light;

typedef struct
{
	Material material;
	float4 position;
	//float radius;
}Sphere;

typedef struct 
{
	float minValue;
	float maxValue;
	bool hit;
}HitReturn;
__constant int OCTREE_MAX_OBJECTS = 5;

typedef struct
{
	
	BBox boundingBox;
	int childrenIndices[8];
	int objectIndices[5];
	float3 origin;
	int numberOfObjects;
}Octree;

uint4 getColor()
{
	return (uint4)(102,32,34,255);
}

bool insideBBox(float3 origin,float3 minPoint, float3 maxPoint)
{
	return (origin.x >= minPoint.x && origin.x <= maxPoint.x) &&
	(origin.y >= minPoint.y && origin.y <= maxPoint.y) &&
	(origin.z >= minPoint.z && origin.z <= maxPoint.z);
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
	returnValue.minValue = minValue;
	returnValue.maxValue = maxValue;
	return returnValue;
}


HitReturn hitBox(Ray ray, BBox box)
{
	return hitBBox(ray,box.min, box.max);
}
HitReturn hitBBoxOctree(Ray ray,float3 minPoint, float3 maxPoint)
{
	float3 t0 = (minPoint - ray.origin) / ray.direction;
	float3 t1 = (maxPoint - ray.origin) /ray.direction;

	float tmin = max(t0.x,max(t0.y,t0.z));
	float tmax = min(t1.x,min(t1.y,t1.z));
	
	HitReturn returnValue;

	returnValue.hit = (tmin < tmax && tmax > epsilion);
	returnValue.minValue = tmin;
	returnValue.maxValue = tmax;

	return returnValue;
}


Ray generateRay(int2 pixelLocation, int width, int height, Camera camera)
{
	Ray ray;
	float2 pixelToRay = (float2)(pixelLocation.x - (0.5f * width -1.0f), pixelLocation.y - (0.5 * (height - 1.0f)));
	ray.origin = camera.position;
	ray.direction = normalize(pixelToRay.x * camera.u + pixelToRay.y * camera.v - camera.distance * camera.w);

	return ray;
}

SphereInfo sphereIntersection(Ray ray, float3 position, float radius)
{
	float3 originToShape = ray.origin - position;
	float a = dot(ray.direction, ray.direction);
	float b = 2.0f * dot(originToShape, ray.direction);
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
			s.normal = (float3) (((ray.origin - position) + t * ray.direction)/ radius);
			s.intersectionPoint = (ray.origin - position) + t * ray.direction;
			s.hasIntersection = true;
			s.distanceToIntersection = t;
			return s;
		}

		t = (-b + e) / denom;
		if(t > epsilion)
		{
			s.normal = (float3)(((ray.origin - position) + t * ray.direction)/radius);
			s.hasIntersection = true;
			s.intersectionPoint = (ray.origin - position) + t * ray.direction;
			s.distanceToIntersection = t;
			return s;
		}
	}

	return s;
}

uint4 adsLight(Object object,Light light,SphereInfo sphereInfo)
{
	float3 ambient = object.material.ambient.xyz * light.material.ambient.xyz;
	float3 lightVector = normalize( light.position.xyz - object.position.xyz);
	float lightDotNormal = max(dot(lightVector.xyz,sphereInfo.normal.xyz), 0.0);
	float3 diffuse = object.material.diffuse.xyz * light.material.diffuse.xyz * lightDotNormal;
	float3 finalColor =   ambient + diffuse;// + specular;
	return (uint4)((convert_uint3(finalColor * 255.0f)),255);
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

bool bboxObjectCollided(BBox b1 , Object o1)
{

	BBox box;
	box.min = o1.box.min + o1.position;
	box.max = o1.box.max + o1.position;

	return bboxCollided(b1,box);

}