
__kernel void sortMortonCodes(__global MortonNode * codes, uint stage, uint pass)
{
	uint codeIndex = get_global_id(0);
	uint pairDistance = 1 << (stage - pass);
	uint blockWidth  = 2 * pairDistance;
	
	uint leftId = (codeIndex & (pairDistance-1)) + (codeIndex >> (stage - pass)) * blockWidth;
	uint rightId = leftId + pairDistance;
	uint temp;

	MortonNode leftNode,rightNode;
	leftNode = codes[leftId];
	rightNode = codes[rightId];

	uint sameDiretionBlockWidth = codeIndex >> stage;
	uint sameDirection = sameDiretionBlockWidth & 0x1;

	temp = sameDirection ? rightId : temp;
	rightId = sameDirection ? leftId : rightId;
	leftId = sameDirection ? temp : leftId;

	MortonNode greater, lesser;

	bool result = (leftNode.code < rightNode.code);

	greater = result?rightNode:leftNode; 
	lesser = result?leftNode:rightNode; 
	codes[ leftId] = lesser;
	codes[ rightId] = greater;

}
/*
__kernel void sortMortonCodes(__global const MortonNode * in,__global MortonNode * out,__local MortonNode * aux)
{
  int i = get_local_id(0); // index in workgroup
  int wg = get_local_size(0); // workgroup size = block size, power of 2

  // Move IN, OUT to block start
  int offset = get_group_id(0) * wg;
  in += offset; out += offset;

  // Load block in AUX[WG]
  aux[i] = in[i];
  barrier(CLK_LOCAL_MEM_FENCE); // make sure AUX is entirely up to date

  // Loop on sorted sequence length
  for (int length=1;length<wg;length<<=1)
  {
    bool direction = ((i & (length<<1)) != 0); // direction of sort: 0=asc, 1=desc
    // Loop on comparison distance (between keys)
    for (int inc=length;inc>0;inc>>=1)
    {
      int j = i ^ inc; // sibling to compare
      MortonNode iData = aux[i];
      uint iKey = iData.code;
      MortonNode jData = aux[j];
      uint jKey = jData.code;
      bool smaller = (jKey < iKey) || ( jKey == iKey && j < i );
      bool swap = smaller ^ (j < i) ^ direction;
      barrier(CLK_LOCAL_MEM_FENCE);
      aux[i] = (swap)?jData:iData;
      barrier(CLK_LOCAL_MEM_FENCE);
    }
  }

  // Write output
  out[i] = aux[i];
}*/