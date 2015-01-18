__kernel void sumCells(__global int * cellsValues, __local int * partialSums,int size, __global int * sum)
{
	int cellIndex = get_global_id(0);
	int accumulator = 0.0;
	//partialSums[cellIndex] = 0;

	while(cellIndex < size)
	{
		int element = cellsValues[cellIndex];
		accumulator = accumulator + element;
		cellIndex += get_global_size(0);
	}

	int local_index= get_local_id(0);
	partialSums[local_index] = accumulator;
	barrier(CLK_LOCAL_MEM_FENCE);
	 for(int offset = get_local_size(0) / 2; offset > 0; offset = offset / 2) {
       
        if (local_index < offset) 
        {
     		int other = partialSums[local_index + offset];
      		int mine = partialSums[local_index];
      		partialSums[local_index] = mine + other;

        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    if (local_index == 0) {
    	sum[0] = partialSums[0];
    }
}
