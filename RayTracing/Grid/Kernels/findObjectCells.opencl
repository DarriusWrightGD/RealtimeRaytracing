__kernel void findObjectCells(__global Object * objects,__global BBox * box,
	__global int * cells,
 __global int * cellIndices,__global int * objectIndices ,
	 __global int * cellIndexIncrement , float3 delta, float3 deltaInv, float3 widthInverse , float3 numberOfVoxels,
	 __global Mesh * meshes)
{
	int objectIndex = get_global_id(0);	

	BBox oBox = objects[objectIndex].box;
	float3 position = meshes[objects[objectIndex].meshIndex].position.xyz;
	int3 cellMin = positionToVoxel(oBox.min + position,widthInverse, numberOfVoxels,box[0] );
	int3 cellMax = positionToVoxel(oBox.max + position,widthInverse, numberOfVoxels,box[0] );

	for(int z = cellMin.z; z <= cellMax.z; z++)
	{
		for(int y = cellMin.y; y <= cellMax.y; y++)
		{
			for(int x = cellMin.x; x <= cellMax.x; x++)
			{
				int cellsIndex = findVoxelIndex((int3)(x,y,z) , numberOfVoxels);
				int currentIndex = (cellsIndex != 0) ?  cellIndices[cellsIndex-1] : 0;
				int offsetIndex = currentIndex + (atom_inc(&cellIndexIncrement[cellsIndex]));
				objectIndices[offsetIndex] = objectIndex + 1;
			}
		}

	}
}