void RenderWindow::updateDrawScene()
{
	setUpDrawSceneArgs();
	err |= clEnqueueNDRangeKernel(queue, drawSceneKernel, 2,
		NULL, &globalWorkSize[0], 
		NULL, 0, NULL, NULL);

	err |= clEnqueueReadImage(queue,writeCLImage,CL_TRUE,origin,region,0,0,readBuffer,0,NULL,NULL);
}
void RenderWindow::updateScene()
{
	releaseUpdate();
	timer.start();	
	handleKeyInput();
	camera.update();

	// 0
	profileTimer.start();

	updateBBox();
	updateCells();
	updateDrawScene();

	imageLabel.setPixmap(QPixmap::fromImage(readImage));
	float drawTimer = profileTimer.elapsed()/1000.0f;

	interval = (timer.elapsed()/1000.0f);
	fps = 1.0f/interval;

}
void RenderWindow::updateBBox()
{
	BBox b = {glm::vec3(MIN,MIN,MIN),0.0f,glm::vec3(MAX,MAX,MAX),0.0f};
	box = b;

	numberOfObjects = objects.size();
	int globalObjectSize = nextPowerOfTwo(objects.size());

	minArr.resize(globalObjectSize);
	maxArr.resize(globalObjectSize);

	minMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &minArr[0],&err);
	maxMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &maxArr[0],&err);

	//tosee
	objectMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Object)* objects.size(), &objects[0],&err);
	if(triangles.size())
		trianglesMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Triangle)* triangles.size(), &triangles[0],&err);
	lightMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Light) * lights.size(), &lights[0],&err);
	boundingBoxMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(BBox), &box,&err);

	//err |= clEnqueueReadBuffer(queue,trianglesMem, CL_TRUE,0, sizeof(Triangle) * triangles.size(), &triangles[0], 0,0,0);
	setUpSceneBoxArgs();


	if(err != CL_SUCCESS) {
		perror("Couldn't set the sceneBBoxKernel argument");
		exit(1);   
	} 

	sceneBBoxGlobalWorkSize =nextPowerOfTwo(objects.size());
	err |= clEnqueueNDRangeKernel(queue, sceneBBoxKernel, 1 ,
		NULL, &sceneBBoxGlobalWorkSize, 
		NULL, 0, NULL, NULL);

	err |=clEnqueueReadBuffer(queue,boundingBoxMem,CL_TRUE,0, sizeof(BBox), &box,0,0,0);



}
void RenderWindow::updateCells()
{
	initCellWorkSize = objects.size();
	calculateVoxelSize();


	numberOfCellObjects = 0;
	cells.resize(totalVoxels);
	cellsMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int) * totalVoxels, &cells[0],&err);
	sumMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int), &numberOfCellObjects,&err);
	//1391 = 1
	setUpCellArgs();

	if(err != CL_SUCCESS) {
		perror("Couldn't set the initializeCellsKernel argument");
		exit(1);   
	}


	err|=clEnqueueNDRangeKernel(queue, initializeCellsKernel, 1 ,
		NULL, &initCellWorkSize, 
		NULL, 0, NULL, NULL);

	err|=clEnqueueReadBuffer(queue,cellsMem,CL_TRUE,0, sizeof(int) * totalVoxels, &cells[0],0,0,0);
	err|=clEnqueueReadBuffer(queue,sumMem,CL_TRUE,0, sizeof(cl_int), &numberOfCellObjects,0,0,0);


	cellIndices.resize(totalVoxels);


	cellIndices[0] = cells[0]; 
	for (int i = 1; i < totalVoxels; i++)
	{
		cellIndices[i] = cells[i] + cellIndices[i-1];
	}

	cellIncrements.resize(totalVoxels);
	objectIndices.resize(numberOfCellObjects);


	cellIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, cellIndices.size() * sizeof(cl_int),&cellIndices[0], &err);
	objectIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * numberOfCellObjects, &objectIndices[0], &err );
	cellIncrementsMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int)* totalVoxels, &cellIncrements[0], &err);

	setUpCellObjectArgs();
	err|=clEnqueueNDRangeKernel(queue, findObjectCellsKernel, 1 ,
		NULL, &initCellWorkSize, 
		NULL, 0, NULL, NULL);

}