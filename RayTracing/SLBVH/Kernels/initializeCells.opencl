/*
__kernel void initializeCells(__global Object * objects,__global BBox * box,__global int * cells,
	int nx, int ny ,int nz,__global BBox * cBox, __global int * totalCells
	)
{
	int objectIndex = get_global_id(0);	
	int cellsIndex = get_global_id(1);	

	BBox cellBox;
	float3 numberOfCellsVector = (float3)(nx,ny,nz);
	float3 minLocation;
	minLocation.z = cellsIndex / (nx* ny);
	minLocation.y = (cellsIndex % (nx* ny)) / nx;
	minLocation.x = cellsIndex - minLocation.y * nx - minLocation.z *nx *ny;

	float3 cellDimensions = (float3)(nx,ny,nz);
	float3 delta = (box[0].max - box[0].min)/cellDimensions ;

	cellBox.min = (float3)(box[0].min + (minLocation * delta));
	cellBox.max = (float3)(box[0].min + delta + (minLocation * delta));

	cBox[cellsIndex].min = cellBox.min;
	cBox[cellsIndex].max = cellBox.max;

	if(bboxObjectCollided(cellBox,objects[objectIndex]))
	{
		atom_inc(&cells[cellsIndex]);
		atom_inc(&totalCells[0]);
	}
	
}

*/
__kernel void initializeCells(__global Object * objects,__global BBox * box,__global int * cells,
	int nx, int ny ,int nz,__global BBox * cBox, __global int * totalCells
	)
{
	int objectIndex = get_global_id(0);	
	int cellsIndex = get_global_id(1);	

	BBox cellBox;
	float3 numberOfCellsVector = (float3)(nx,ny,nz);
	float3 cellSize = (float3)(fabs(box[0].max.x - box[0].min.x),fabs(box[0].max.y - box[0].min.y),fabs(box[0].max.z - box[0].min.z))/numberOfCellsVector;
	float3 minLocation;
	minLocation.z = cellsIndex / (nx* ny);
	minLocation.y = (cellsIndex % (nx* ny)) / nx;
	minLocation.x = cellsIndex - minLocation.y * nx - minLocation.z *nx *ny;

	cellBox.min = (float3)(box[0].min + (minLocation * cellSize));
	cellBox.max = (float3)(box[0].min + cellSize + (minLocation * cellSize));

	cBox[cellsIndex].min = cellBox.min;
	cBox[cellsIndex].max = cellBox.max;

	if(bboxObjectCollided(cellBox,objects[objectIndex]))
	{
		atom_inc(&cells[cellsIndex]);
		atom_inc(&totalCells[0]);
	}
	
}

