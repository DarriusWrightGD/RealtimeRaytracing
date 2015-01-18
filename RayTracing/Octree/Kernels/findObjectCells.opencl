__kernel void findObjectCells(__global Object * objects,__global BBox * box,
	__global int * cells,
 __global int * cellIndices,__global int * objectIndices ,
	__global BBox * cBox, __global int * cellIndexIncrement)
{
	int objectIndex = get_global_id(0);	
	int cellsIndex = get_global_id(1);	

	if(bboxObjectCollided(cBox[cellsIndex],objects[objectIndex]) )
	{
		int currentIndex = (cellsIndex == 0) ?  0 : cellIndices[cellsIndex-1];
		bool found = false;

		int offsetIndex = currentIndex + (atom_inc(&cellIndexIncrement[cellsIndex]));

		objectIndices[offsetIndex] = objectIndex + 1;	
			
		
		
	}
}