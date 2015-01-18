__kernel void generateSceneBBox(__global BBox * box, __global Object * objects,
	 int length,
     __global float3 * minArray,
     __global float3 * maxArray , __global Mesh * meshes)
{


 	int global_index = get_global_id(0);

    float3 accumulatorMin = (float3)(INFINITY,INFINITY,INFINITY);
    float3 accumulatorMax = (float3)(-INFINITY,-INFINITY,-INFINITY);
    
    // Loop sequentially over chunks of input vector
    while (global_index < length) {
        
        float3 position = meshes[objects[global_index].meshIndex].position.xyz;
        accumulatorMax = max(accumulatorMax, objects[global_index].box.max + position);
        accumulatorMin = min(accumulatorMin, objects[global_index].box.min + position);
       
        global_index += get_global_size(0);
    }

    // Perform parallel reduction
    int local_index = get_local_id(0);
    maxArray[local_index] = accumulatorMax;
    minArray[local_index] = accumulatorMin;

    barrier(CLK_GLOBAL_MEM_FENCE);
    for(int offset = get_local_size(0) / 2; offset > 0; offset = offset / 2) 
    {
        if (local_index < offset) {

            float3 otherMax = maxArray[local_index + offset];
            float3 mineMax = maxArray[local_index];
            maxArray[local_index] = max(mineMax, otherMax);

            float3 otherMin = minArray[local_index + offset];
            float3 mineMin = minArray[local_index];
            minArray[local_index] = min(mineMin, otherMin);
                    
        }
        barrier(CLK_GLOBAL_MEM_FENCE);
    }
    if (local_index == 0) 
    {
        box[0].max = maxArray[0] + 0.6f;
        box[0].min = minArray[0] - 0.6f;

    }
}