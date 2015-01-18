

typedef struct
{
	float3 color;
	float3 reflection;
}BRDFRet;

typedef struct BRDF
{
	float kd;
	float3 cd;
	uint type;
}BRDF;

typedef struct
{
	float3 direction;
	float3 position;
	float3 color;
	float distance;
	int objectIndex;
	int type;
}IntersectionInfo;

typedef struct
{
	float3 direction;
	float3 position;
	float distance;
	int objectIndex;
	int type;
	int octreeNode;
	int endDepth;
}IntersectionInfoO;

typedef struct
{
	float3 min;
	float3 max;
}BBox;

typedef struct
{
	BBox boundingBox;
	int nodes[2];
	int startingIndex;
	int numberOfObjects;
	float d[1];
	int parent;
	int index;
	int depth;
}KDNode;

typedef struct
{
	float3 nextCrossing;
	float3 deltaT;
	int3 nextStep;
	int3 out;
}StepInfo;

typedef struct
{
	StepInfo stepInfo;
	int3 currentCell;
	bool continueStep;

}StepReturn;

typedef struct 
{
	float4 color;
	float reflection;
	float refraction;
	uint type;
	float d[1];
}Material;

typedef struct
{
	float3 v0;
	float3 v1;
	float3 v2;
	float3 normal;
}Triangle;

typedef struct 
{
	float3 v0;
	float3 v1;
	float3 v2;
	float3 normal;
	bool hasIntersection;
	float distanceFromIntersection;

}TriangleInfo;

typedef struct
{
    BBox box;
    int triangleIndex;
    uint objectIndex;
    uint meshIndex;
    float d[1];
}Object;

typedef struct
{
	Material material;
	float4 position;
	uint2 indices;
	float d[2];
}Mesh;

typedef struct
{
	float3 u;
	float3 v;
	float3 w;
	float3 up;
	float3 lookAt;
	float4 position;
	float distance;
	float zoom;
	uint type;
	float focalDistance;
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
	float4 direction;
	int type;
	float d[3]
}Light;

typedef struct
{
	Material material;
	float4 position;
}Sphere;

typedef struct 
{
	float minValue;
	float maxValue;
	bool hit;
}HitReturn;

typedef struct
{
	BBox boundingBox;
	int objectIndices[5];
	float origin[3];
	int index;
	int numberOfObjects;
	int childrenStart;
	int depth;
}Octree;