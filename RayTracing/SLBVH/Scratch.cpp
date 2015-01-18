#include "RenderWindow.h"

/*
CL_DEVICE_MAX_WORK_ITEM_SIZES - 1024, 1024, 64
*/
const cl_float RenderWindow::MIN = 10000000.0f;
const cl_float RenderWindow::MAX = -10000000.0f;
const cl_uint RenderWindow::NUMBER_OF_SPHERES = 2000;

RenderWindow::RenderWindow(void) : multi(2.0f),camera(glm::vec3(0,0,300), glm::vec3(0,0,0)), radixGroupSize(64), radixBinSize(256),radixDataSize(0)
{
	BBox b = {glm::vec3(MIN,MIN,MIN),0.0f,glm::vec3(MAX,MAX,MAX),0.0f};
	box = b;
	construct();
}
RenderWindow::~RenderWindow(void)
{
	delete[]readBuffer;
	clReleaseMemObject(clImage);

	clReleaseMemObject(writeCLImage);
	clReleaseSampler(sampler);
	clReleaseKernel(drawSceneKernel);
	clReleaseKernel(sceneBBoxKernel);
	clReleaseKernel(initializeCellsKernel);
	clReleaseKernel(findObjectCellsKernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseContext(context);

	releaseUpdate();

}
void RenderWindow::resizeEvent(QResizeEvent * e)
{
	//globalWorkSize[0] = roundUp( localWorkSize[0], width());
	//globalWorkSize[1] = roundUp( localWorkSize[0], height());
	windowWidth = width();
	windowHeight = height();
	delete [] readBuffer;
	readBuffer = new uchar [width() * height() * 4];

}
void RenderWindow::addLight(Light light)
{
	lights.push_back(light);
	numberOfLights++;
}
void RenderWindow::addObject(Object object)
{
	objects.push_back(object);
	numberOfObjects++;
}



void RenderWindow::computeRadixGroups()
{
	radixDataSize = (objects.size() < 512) ? 512 : nextPowerOfTwo(objects.size());

	radixBinSize = (radixDataSize <= radixBinSize) ? radixDataSize/2 : radixBinSize;

	if ((radixDataSize / radixBinSize)/radixGroupSize  <=1)
	{
		radixGroupSize = (radixDataSize / radixBinSize)/2;
		if(radixGroupSize == 0)
		{
			radixGroupSize = 1;
		}

	}
}

void  RenderWindow::computeHistogram(int currentByte) {
	cl_int status;
	size_t globalThreads = radixDataSize;
	size_t localThreads  = radixBinSize;


	status = clSetKernelArg(histogramKernel, 0, sizeof(cl_mem), (void*)&mortonMem);
	status = clSetKernelArg(histogramKernel, 1, sizeof(cl_mem), (void*)&histogramMem);
	status = clSetKernelArg(histogramKernel, 2, sizeof(cl_int), (void*)&currentByte);
	status = clSetKernelArg(histogramKernel, 3, sizeof(cl_int) * radixBinSize, NULL); 
	status = clEnqueueNDRangeKernel(
		queue, 
		histogramKernel,
		1,
		NULL,
		&globalThreads,
		&localThreads,
		0,
		NULL,NULL);
}

void  RenderWindow::computeRankingNPermutations(int currentByte, size_t groupSize) {
	cl_int status;

	size_t globalThreads = radixDataSize/R;
	size_t localThreads  = groupSize;

	status = clSetKernelArg(permuteKernel, 0, sizeof(cl_mem), (void*)&mortonMem);
	status = clSetKernelArg(permuteKernel, 1, sizeof(cl_mem), (void*)&scannedHistogramMem);
	status = clSetKernelArg(permuteKernel, 2, sizeof(cl_int), (void*)&currentByte);
	status = clSetKernelArg(permuteKernel, 3, groupSize * R * sizeof(cl_ushort), NULL);
	status = clSetKernelArg(permuteKernel, 4, sizeof(cl_mem), (void*)&sortedDataMem);

	status = clEnqueueNDRangeKernel(queue, permuteKernel, 1, NULL, &globalThreads, &localThreads, 0, NULL, NULL);
	status = clEnqueueCopyBuffer(queue, sortedDataMem, mortonMem, 0, 0, radixDataSize * sizeof(MortonNode), 0, NULL, NULL);
}

void  RenderWindow::computeBlockScans() {
	cl_int status;

	size_t numberOfGroups = radixDataSize / radixBinSize;
	size_t globalThreads[2] = {numberOfGroups, R};
	size_t localThreads[2]  = {radixGroupSize, 1};
	cl_uint groupSize = radixGroupSize;

	status = clSetKernelArg(blockScanKernel, 0, sizeof(cl_mem), (void*)&scannedHistogramMem);
	status = clSetKernelArg(blockScanKernel, 1, sizeof(cl_mem), (void*)&histogramMem);
	status = clSetKernelArg(blockScanKernel, 2, radixGroupSize * sizeof(cl_uint), NULL);
	status = clSetKernelArg(blockScanKernel, 3, sizeof(cl_uint), &groupSize); 
	status = clSetKernelArg(blockScanKernel, 4, sizeof(cl_mem), &sumInMem);


	status = clEnqueueNDRangeKernel(
		queue,
		blockScanKernel,
		2,
		NULL,
		globalThreads,
		localThreads,
		0, 
		NULL,
		NULL);





	if(numberOfGroups/radixGroupSize != 1) {
		size_t globalThreadsPrefix[2] = {numberOfGroups/radixGroupSize, R};
		status = clSetKernelArg(prefixSumKernel, 0, sizeof(cl_mem), (void*)&sumOutMem);

		status = clSetKernelArg(prefixSumKernel, 1, sizeof(cl_mem), (void*)&sumInMem);

		status = clSetKernelArg(prefixSumKernel, 2, sizeof(cl_mem), (void*)&summaryInMem);

		cl_uint stride = (cl_uint)numberOfGroups/radixGroupSize;
		status = clSetKernelArg(prefixSumKernel, 3, sizeof(cl_uint), (void*)&stride);

		status = clEnqueueNDRangeKernel(
			queue,
			prefixSumKernel,
			2,
			NULL,
			globalThreadsPrefix,
			NULL,
			0,
			NULL,
			NULL);



		size_t globalThreadsAdd[2] = {numberOfGroups, R};
		size_t localThreadsAdd[2]  = {radixGroupSize, 1};
		status = clSetKernelArg(blockAddKernel, 0, sizeof(cl_mem), (void*)&sumOutMem);  

		status = clSetKernelArg(blockAddKernel, 1, sizeof(cl_mem), (void*)&scannedHistogramMem);  

		status = clSetKernelArg(blockAddKernel, 2, sizeof(cl_uint), (void*)&stride);  

		status = clEnqueueNDRangeKernel(
			queue,
			blockAddKernel,
			2,
			NULL,
			globalThreadsAdd,
			localThreadsAdd,
			0,
			NULL,
			NULL);


		size_t globalThreadsScan[1] = {R};
		size_t localThreadsScan[1] = {R};
		status = clSetKernelArg(unifiedBlockScanKernel, 0, sizeof(cl_mem), (void*)&summaryOutMem);

		if(numberOfGroups/radixGroupSize != 1) 
			status = clSetKernelArg(unifiedBlockScanKernel, 1, sizeof(cl_mem), (void*)&summaryInMem); 
		else
			status = clSetKernelArg(unifiedBlockScanKernel, 1, sizeof(cl_mem), (void*)&sumInMem); 


		status = clSetKernelArg(unifiedBlockScanKernel, 2, R * sizeof(cl_uint), NULL);

		groupSize = R;
		status = clSetKernelArg(unifiedBlockScanKernel, 3, sizeof(cl_uint), (void*)&groupSize); 

		status = clEnqueueNDRangeKernel(
			queue,
			unifiedBlockScanKernel,
			1,
			NULL,
			globalThreadsScan,
			localThreadsScan,
			0, 
			NULL, 
			NULL);




		size_t globalThreadsOffset[2] = {numberOfGroups, R};
		status = clSetKernelArg(mergePrefixSumsKernel, 0, sizeof(cl_mem), (void*)&summaryOutMem);

		status = clSetKernelArg(mergePrefixSumsKernel, 1, sizeof(cl_mem), (void*)&scannedHistogramMem);

		status = clEnqueueNDRangeKernel(queue, mergePrefixSumsKernel, 2, NULL, globalThreadsOffset, NULL, 0, NULL, NULL);

	}
}


void RenderWindow::radixSort() 
{
	computeRadixGroups();
	initializeMortonMem();
	int size = radixDataSize;

	//std::vector<MortonNode> sortedMortonCodes;
	//sortedMortonCodes.resize(size);

	for(int currentByte = 0; currentByte < sizeof(cl_uint) * bitsbyte ; currentByte += bitsbyte) {
		computeHistogram(currentByte);
		computeBlockScans();
		computeRankingNPermutations(currentByte, radixGroupSize);



	}
	//clEnqueueReadBuffer(queue,sortedDataMem,CL_TRUE,0, radixDataSize *  sizeof(MortonNode) , &sortedMortonCodes[0],0,0,0);
}

void RenderWindow::releaseRadixMem()
{
	
	clReleaseMemObject(histogramMem       );
	clReleaseMemObject(scannedHistogramMem);
	clReleaseMemObject(sortedDataMem      );
	clReleaseMemObject(sumInMem           );
	clReleaseMemObject(sumOutMem          );
	clReleaseMemObject(summaryInMem       );
	clReleaseMemObject(summaryOutMem      );
}


void RenderWindow::construct()
{
	setMinimumSize(640,480);

	layout = new QHBoxLayout();
	Light light = {{{0.925,0.835,0.102}, {0.73,0.724,0.934},{0.2,0.52,0.96}}, {2.0f,2.0f,200.0f}};


	Random random = Random::getInstance();
	for(int i = 0; i < NUMBER_OF_SPHERES; i++)
	{

		Sphere sphere = {{{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}, 
		{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)},
		{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}},
		glm::vec4(random.getRandomFloat(-100.0f,100.0f), random.getRandomFloat(-100.0f,100.0f), random.getRandomFloat(30.0f,50.0f),30.0f)};
		//spheres.push_back(sphere);

		Object object; 

		BBox b = {
			glm::vec3(sphere.getMinX(),
			sphere.getMinY(),
			sphere.getMinZ()),0.0f,
			glm::vec3(
			sphere.getMaxX(),
			sphere.getMaxY(),
			sphere.getMaxZ()),0.0f

		};
		object.material = sphere.material;
		object.box = b;
		cl_int2 indices = {0,0};
		object.triangleIndices =indices;
		object.index = i;
		object.position = sphere.position;


		objects.push_back(object);
		float x;
		float y;

		box.min[0] = glm::min((sphere.getMinX() + sphere.position[0]) ,box.min[0]);
		box.min[1] = glm::min(sphere.getMinY()  +  sphere.position[1],box.min[1]);
		box.min[2] = glm::min(sphere.getMinZ()	 + sphere.position[2],box.min[2]);

		box.max[0] = glm::max(sphere.getMaxX()+ sphere.position[0],box.max[0]);
		box.max[1] = glm::max(sphere.getMaxY()+  sphere.position[1],box.max[1]);
		box.max[2] = glm::max(sphere.getMaxZ()+ sphere.position[2],box.max[2]);


	}




	lights.push_back(light);
	numberOfObjects = objects.size();
	numberOfLights = lights.size();

	layout->addWidget(&imageLabel);
	setLayout(layout);

	windowWidth = width();
	windowHeight = height();
	readBuffer = new uchar [windowWidth * windowHeight * 4];
	initializeCL();

	connect(&updateTimer,SIGNAL(timeout()),this,SLOT(updateScene()));
	updateTimer.start();

	QImage r(readBuffer,windowWidth,windowHeight,QImage::Format::Format_RGBA8888);
	readImage  =r;
}
void RenderWindow::updateDrawScene()
{
	err |= clSetKernelArg(drawSceneKernel,0 , sizeof(cl_mem), &clImage);
	err |= clSetKernelArg(drawSceneKernel,1 , sizeof(cl_mem), &writeCLImage);
	err |= clSetKernelArg(drawSceneKernel,2 , sizeof(cl_sampler), &sampler);
	err |= clSetKernelArg(drawSceneKernel,3 , sizeof(cl_int), &windowWidth);
	err |= clSetKernelArg(drawSceneKernel,4 , sizeof(cl_int), &windowHeight);
	err |= clSetKernelArg(drawSceneKernel,5 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(drawSceneKernel,6 , sizeof(cl_mem), &lightMem);
	err |= clSetKernelArg(drawSceneKernel,7 , sizeof(cl_int), &numberOfObjects);
	err |= clSetKernelArg(drawSceneKernel,8 , sizeof(cl_int), &numberOfLights);
	err |= clSetKernelArg(drawSceneKernel,9, sizeof(BBox), &box);
	err |= clSetKernelArg(drawSceneKernel, 10, sizeof(cl_mem), &cellsMem);
	err |= clSetKernelArg(drawSceneKernel,11 , sizeof(cl_int), &nx);
	err |= clSetKernelArg(drawSceneKernel,12 , sizeof(cl_int), &ny);
	err |= clSetKernelArg(drawSceneKernel,13 , sizeof(cl_int), &nz);
	err |= clSetKernelArg(drawSceneKernel, 14, sizeof(cl_mem), &cellIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 15, sizeof(cl_mem), &objectIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 16, sizeof(Camera), &camera);


	clEnqueueNDRangeKernel(queue, drawSceneKernel, 2,
		NULL, globalWorkSize, 
		NULL, 0, NULL, NULL);

	clEnqueueReadImage(queue,writeCLImage,CL_TRUE,origin,region,0,0,readBuffer,0,NULL,NULL);
}
void RenderWindow::updateScene()
{
	//releaseUpdate();
	timer.start();	
	handleKeyInput();
	camera.update();

	// 0
	//updateBBox();
	//updateCells();

	profileTimer.start();
	updateDrawScene();
	float drawTimer = profileTimer.elapsed()/1000.0f;

	imageLabel.setPixmap(QPixmap::fromImage(readImage));

	float time = (timer.elapsed()/1000.0f);
	float frames = 1.0f/time;
	setWindowTitle( "Frames : "+ QString::number(frames));



}
void RenderWindow::updateBBox()
{
	BBox b = {glm::vec3(MIN,MIN,MIN),0.0f,glm::vec3(MAX,MAX,MAX),0.0f};

	box = b;
	objectMem = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR,sizeof(Object)* objects.size(), &objects[0],0);
	lightMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Light), &lights[0],0);
	boundingBoxMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,sizeof(BBox), &box,&err);
	lengthOfObjects = objects.size();
	clSetKernelArg(sceneBBoxKernel,0 , sizeof(cl_mem), &boundingBoxMem);
	clSetKernelArg(sceneBBoxKernel,1 , sizeof(cl_mem), &objectMem);
	clSetKernelArg(sceneBBoxKernel,2 , sizeof(cl_int), &lengthOfObjects);
	clSetKernelArg(sceneBBoxKernel,3 , sizeof(cl_float3) * objects.size(),NULL);
	clSetKernelArg(sceneBBoxKernel,4 , sizeof(cl_float3) * objects.size(),NULL);

	sceneBBoxGlobalWorkSize = objects.size();
	clEnqueueNDRangeKernel(queue, sceneBBoxKernel, 1 ,
		NULL, &sceneBBoxGlobalWorkSize, 
		NULL, 0, NULL, NULL);

	clEnqueueReadBuffer(queue,boundingBoxMem,CL_TRUE,0, sizeof(BBox), &box,0,0,0);




}
void RenderWindow::updateCells()
{
	initCellWorkSize[0] = objects.size();

	wx =box.max[0] - box.min[0];
	wy =box.max[1] - box.min[1];
	wz =box.max[2] - box.min[2];

	s = pow((wx * wy * wz)/objects.size(), 0.3333333);

	nx = multi * wx / s + 1;
	ny = multi * wy / s + 1;
	nz = multi * wz / s + 1;

	numCells = nx * ny * nz;
	numCells = numCells;


	cells.resize(numCells);
	cBoxes.resize(numCells);
	initCellWorkSize[1] = numCells;
	numberOfCellObjects = 0;
	cellsMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int) * numCells, &cells[0],&err);
	cellsBoxMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(BBox) * numCells, &cBoxes[0],&err);
	sumMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int), &numberOfCellObjects,&err);


	clSetKernelArg(initializeCellsKernel,0 , sizeof(cl_mem), &objectMem);
	clSetKernelArg(initializeCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	clSetKernelArg(initializeCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	clSetKernelArg(initializeCellsKernel,3 , sizeof(cl_int), &nx);
	clSetKernelArg(initializeCellsKernel,4 , sizeof(cl_int), &ny);
	clSetKernelArg(initializeCellsKernel,5 , sizeof(cl_int), &nz);
	clSetKernelArg(initializeCellsKernel, 6, sizeof(cl_mem),&cellsBoxMem );
	clSetKernelArg(initializeCellsKernel, 7, sizeof(cl_mem), &sumMem );


	clEnqueueNDRangeKernel(queue, initializeCellsKernel, 2 ,
		NULL, initCellWorkSize, 
		NULL, 0, NULL, NULL);

	clEnqueueReadBuffer(queue,cellsMem,CL_TRUE,0, sizeof(int) * numCells, &cells[0],0,0,0);
	clEnqueueReadBuffer(queue,cellsBoxMem,CL_TRUE,0, sizeof(BBox) * numCells, &cBoxes[0],0,0,0);
	clEnqueueReadBuffer(queue,sumMem,CL_TRUE,0, sizeof(cl_int), &numberOfCellObjects,0,0,0);


	cellIndices.resize(numCells);


	cellIndices[0] = cells[0]; 
	for (int i = 1; i < numCells; i++)
	{
		cellIndices[i] = cells[i] + cellIndices[i-1];
	}

	cellIncrements.resize(numCells);
	objectIndices.resize(numberOfCellObjects);


	cellIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, cellIndices.size() * sizeof(cl_int),&cellIndices[0], &err);
	objectIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * numberOfCellObjects, &objectIndices[0], &err );
	cellIncrementsMem = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR, sizeof(cl_int)* numCells, &cellIncrements[0], &err);


	clSetKernelArg(findObjectCellsKernel,0 , sizeof(cl_mem), &objectMem);
	clSetKernelArg(findObjectCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	clSetKernelArg(findObjectCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	clSetKernelArg(findObjectCellsKernel,3 , sizeof(cl_mem) , &cellIndicesMem);
	clSetKernelArg(findObjectCellsKernel,4 , sizeof(cl_mem)  , &objectIndicesMem);
	clSetKernelArg(findObjectCellsKernel,5 , sizeof(cl_mem) , &cellsBoxMem);
	clSetKernelArg(findObjectCellsKernel,6 , sizeof(cl_mem) , &cellIncrementsMem);
	clEnqueueNDRangeKernel(queue, findObjectCellsKernel, 2 ,
		NULL, initCellWorkSize, 
		NULL, 0, NULL, NULL);

	clEnqueueReadBuffer(queue,objectIndicesMem,CL_TRUE,0, sizeof(int) * numberOfCellObjects, &objectIndices[0],0,0,0);
	clEnqueueReadBuffer(queue,cellIncrementsMem,CL_TRUE,0,sizeof(int) * numCells, &cellIncrements[0],0,0,0);

	//vector<cl_int>tObjectIndices;
	//tObjectIndices.resize(numberOfCellObjects);

	//for(int j = 0; j < cBoxes.size(); j++)
	//{
	//	for (int i = 0; i < objects.size(); i++)
	//	{
	//		if(objectBoxCollided(cBoxes[j], objects[i]))
	//		{
	//			int currentIndex = (j == 0) ?  0 : cellIndices[j-1];
	//			while(currentIndex < objectIndices.size() && objectIndices[currentIndex] != 0)
	//			{
	//				currentIndex++;
	//			}
	//			if(currentIndex < objectIndices.size())
	//				objectIndices[currentIndex] = i + 1;
	//		}
	//	}
	//}

	//	objectIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * numberOfCellObjects, &objectIndices[0], &err );


}
void RenderWindow::releaseUpdate()
{
	cellIndices.clear();
	objectIndices.clear();
	cellIncrements.clear();
	cBoxes.clear();
	cells.clear();
	clReleaseMemObject(sumMem);
	clReleaseMemObject(objectMem);//
	clReleaseMemObject(lightMem);//
	clReleaseMemObject(cellsMem);//
	clReleaseMemObject(objectIndicesMem);//
	clReleaseMemObject(cellIndicesMem);//
	clReleaseMemObject(cellIncrementsMem);//
	clReleaseMemObject(cellsBoxMem);//
	clReleaseMemObject(minMem);//
	clReleaseMemObject(maxMem);//
	clReleaseMemObject(mortonMem);

}
cl_program RenderWindow::buildProgram(std::string files [], int numberOfFiles)
{
	/* Identify a platform */
	err = clGetPlatformIDs(1, &platform, NULL);
	if(err < 0) {
		perror("Couldn't find any platforms");
		exit(1);
	}

	/* Access a device */
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, 
		&device, NULL);
	if(err < 0) {
		perror("Couldn't find any devices");
		exit(1);
	}

	/* Create the context */
	context = clCreateContext(NULL, 1, &device, NULL, 
		NULL, &err);
	if(err < 0) {
		perror("Couldn't create a context");
		exit(1);   
	}

	/* Read program file and place content into buffer
	when doing multiple programs for each through them all here
	*/

	char ** program_buffer = new char*[numberOfFiles]; 
	size_t * program_size = new size_t[numberOfFiles];
	for(int i = 0; i < numberOfFiles; i++)
	{
		const char * fileName =files[i].c_str(); 
		program_handle = fopen(fileName, "r");
		if(program_handle == NULL) {
			perror("Couldn't find the program file");
			exit(1);   
		}
		fseek(program_handle, 0, SEEK_END);
		program_size[i] = ftell(program_handle);
		rewind(program_handle);

		// read kernel source into buffer
		program_buffer[i] = (char*) malloc(program_size[i]+ 1);
		program_buffer[i][program_size[i]] = '\0';
		fread(program_buffer[i], sizeof(char), program_size[i] +1 , program_handle);
		fclose(program_handle);
		program_buffer[i][program_size[i]] = '\0';
		int start = program_size[i]-1;
		while(program_buffer[i][start] == 'Í')
		{
			program_buffer[i][start--] = ' ';
		}

	}
	//end

	/* Create program from file */
	cl_program program = clCreateProgramWithSource(context, numberOfFiles, 
		(const char**)program_buffer, program_size, &err);
	if(err < 0) {
		perror("Couldn't create the program");
		exit(1);   
	}

	for (int i = 0; i < numberOfFiles; i++)
	{
		free(program_buffer[i]);
	}
	delete [] program_buffer;
	delete [] program_size;
	const char options[] = "-cl-std=CL1.1 -cl-mad-enable -Werror";

	/* Build program */
	err = clBuildProgram(program, 0, NULL, 
		options,//compiler options go here see page 32
		NULL, NULL);
	if(err < 0) {

		/* Find size of log and print to std output */
		//different build statuses see page 34
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 
			0, NULL, &log_size);
		program_log = (char*) malloc(log_size + 1);
		program_log[log_size] = '\0';
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 
			log_size + 1, program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);
		exit(1);
	}

	return program;
}
void RenderWindow::handleKeyInput()
{
	if(GetAsyncKeyState('D'))
	{
		camera.moveRight();
	}
	if(GetAsyncKeyState('S'))
	{
		camera.moveDown();
	}
	if(GetAsyncKeyState('W'))
	{
		camera.moveUp();
	}
	if(GetAsyncKeyState('A'))
	{
		camera.moveLeft();
	}
	if(GetAsyncKeyState('R'))
	{
		camera.moveIn();
	}
	if(GetAsyncKeyState('F'))
	{
		camera.moveOut();
	}

}
void RenderWindow::initializeProgram()
{
	std::string files []  = { "Kernels/kernelHelperFunctions.opencl", "Kernels/radixSort.opencl","Kernels/sortMortonCodes.opencl","Kernels/generateMortonsCode.opencl", "Kernels/generateSceneBBox.opencl","Kernels/initializeCells.opencl","Kernels/findObjectCells.opencl", "Kernels/drawScene.opencl"};
	program = buildProgram(files, 6);
	drawSceneKernel = clCreateKernel(program, "drawScene", &err);
	sceneBBoxKernel = clCreateKernel(program, "generateSceneBBox", &err);
	initializeCellsKernel = clCreateKernel(program, "initializeCells", &err);
	findObjectCellsKernel = clCreateKernel(program,"findObjectCells", &err);
	initializeMortonCodesKernel = clCreateKernel(program,"initializeMortonCodes", &err);
	sortMortonKernel = clCreateKernel(program,"sortMortonCodes", &err);
	
	histogramKernel = clCreateKernel(program, "computeHistogram", &err);
	permuteKernel   = clCreateKernel(program, "rankNPermute", &err);
	unifiedBlockScanKernel  = clCreateKernel(program, "unifiedBlockScan", &err);
	blockScanKernel  = clCreateKernel(program, "blockScan", &err);
	prefixSumKernel = clCreateKernel(program, "blockPrefixSum", &err);
	blockAddKernel  = clCreateKernel(program, "blockAdd", &err);
	mergePrefixSumsKernel    = clCreateKernel(program, "mergePrefixSums", &err);
}

void RenderWindow::initializeMemory()
{
	//	QImage image(windowWidth,windowHeight,QImage::Format_RGBA8888);
	//char * buffer = new char[windowWidth * windowHeight * 4];
	//memcpy( buffer,image.bits(), windowWidth* windowHeight * 4);

	cl_image_format clImageFormat;
	clImageFormat.image_channel_order = CL_RGBA;
	clImageFormat.image_channel_data_type = CL_UNSIGNED_INT8;


	objectMem = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR,sizeof(Object)* objects.size(), &objects[0],&err);
	if(err != CL_SUCCESS)
	{
		cout << "Error creating Object" << endl;
		exit(-1);
	}

	lightMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Light)* lights.size(), &lights[0],&err);
	if(err != CL_SUCCESS)
	{
		cout << "Error creating Lights Object" << endl;
		exit(-1);
	}


	clImage = clCreateImage2D(context, CL_MEM_READ_ONLY , & clImageFormat, windowWidth,windowHeight,0,NULL,&err);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating CL Image object" << endl;
		exit(-1);
	}

	writeCLImage = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &clImageFormat, windowWidth,windowHeight, 0 ,NULL, &err);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating writable CL Image object" << endl;
		exit(-1);
	}

	sampler  = clCreateSampler(context, CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST, & err);
	if(err != CL_SUCCESS)
	{
		cout << "Error creating CL sampler object" << endl;
		//cleanup
		exit(-1);
	}

	boundingBoxMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,sizeof(BBox), &box,&err);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating bounding box memory sampler object" << endl;
		//cleanup
		exit(-1);
	}


	if(err < 0) {
		perror("Couldn't create the kernel");
		exit(1);   
	}



	//unsortedDataMem     = clCreateBuffer(context, CL_MEM_READ_ONLY|CL_MEM_USE_HOST_PTR, sizeof(MortonNode) * radixDataSize, unsortedData, &error);





	//delete [] buffer;

}


void RenderWindow::initializeMortonMem()
{

	//radixDataSize = (objects.size() < 512) ? 512 : nextPowerOfTwo(objects.size());
	cl_uint groupSize = radixGroupSize;
	cl_uint numberOfGroups = radixDataSize / (groupSize * R);

	histogramMem        = clCreateBuffer(context, CL_MEM_READ_WRITE, numberOfGroups * groupSize * R * sizeof(cl_uint), NULL, &err);
	scannedHistogramMem = clCreateBuffer(context, CL_MEM_READ_WRITE, numberOfGroups * groupSize * R * sizeof(cl_uint), NULL, &err);
	sortedDataMem       = clCreateBuffer(context, CL_MEM_WRITE_ONLY, radixDataSize * sizeof(MortonNode), NULL, &err);
	sumInMem           = clCreateBuffer(context, CL_MEM_READ_WRITE, (radixDataSize/groupSize) * sizeof(cl_uint), NULL, &err);
	sumOutMem          = clCreateBuffer(context, CL_MEM_READ_WRITE, (radixDataSize/groupSize) * sizeof(cl_uint), NULL, &err);
	summaryInMem       = clCreateBuffer(context, CL_MEM_READ_WRITE, R * sizeof(cl_uint), NULL, &err);
	summaryOutMem      = clCreateBuffer(context, CL_MEM_READ_WRITE, R * sizeof(cl_uint), NULL, &err);
}


void RenderWindow::initializeSceneBBox()
{


	cl_int lengthOfObjects = objects.size();
	int globalObjectSize = nextPowerOfTwo(objects.size());

	vector<cl_float3> minArr;
	minArr.resize(globalObjectSize);
	vector<cl_float3> maxArr;
	maxArr.resize(globalObjectSize);

	minMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &minArr[0],&err);
	maxMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &maxArr[0],&err);
	//finish setting up mem stuff in header file...

	err |= clSetKernelArg(sceneBBoxKernel,0 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(sceneBBoxKernel,1 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(sceneBBoxKernel,2 , sizeof(cl_int), &lengthOfObjects);
	err |= clSetKernelArg(sceneBBoxKernel,3 , sizeof(cl_mem),minMem);
	err |= clSetKernelArg(sceneBBoxKernel,4 , sizeof(cl_mem) , maxMem);


	queue = clCreateCommandQueue(context, device, 0, &err);
	if(err < 0) {
		perror("Couldn't create the command queue");
		exit(1);   
	}

	if(err != CL_SUCCESS) {
		perror("Couldn't set the sceneBBoxKernel argument");
		exit(1);   
	} 

	sceneBBoxGlobalWorkSize = nextPowerOfTwo(objects.size());
	err = clEnqueueNDRangeKernel(queue, sceneBBoxKernel, 1 ,
		NULL, &sceneBBoxGlobalWorkSize, 
		NULL, 0, NULL, NULL);

	clEnqueueReadBuffer(queue,boundingBoxMem,CL_TRUE,0, sizeof(BBox), &box,0,0,0);


}
void RenderWindow::initializeCells()
{

	initCellWorkSize[0] = objects.size();

	mortonNodes.resize(objects.size());

	mortonMem = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR, sizeof(MortonNode) * objects.size(), &mortonNodes[0],&err);
	//	std::vector<cl_float3> position;
	//	position.resize(objects.size());
	//	cl_mem positionMem = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR, sizeof(cl_float3) * objects.size(), &position[0],&err);

	err |= clSetKernelArg(initializeMortonCodesKernel,0, sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(initializeMortonCodesKernel,1, sizeof(cl_mem), &mortonMem);
	//err |= clSetKernelArg(initializeMortonCodesKernel,2, sizeof(cl_mem), &positionMem);

	err = clEnqueueNDRangeKernel(queue, initializeMortonCodesKernel, 1 ,NULL, &initCellWorkSize[0], NULL, 0, NULL, NULL);

	if(err != CL_SUCCESS) {
		perror("Couldn't set the initializeMortonCodesKernel argument");
		exit(1);   
	} 

	clEnqueueReadBuffer(queue,mortonMem,CL_TRUE,0, sizeof(MortonNode) * objects.size(), &mortonNodes[0],0,0,0);



	vector<MortonNode> mortonNodesOut;
	mortonNodesOut.resize(objects.size());


	timer.start();
	
	radixSort();
	float endTime = timer.elapsed()/1000.0f;
	err|= clEnqueueReadBuffer(queue,sortedDataMem,CL_TRUE,0, sizeof(MortonNode) * objects.size(), &mortonNodesOut[0],0,0,0);

	releaseRadixMem();

	//	err |= clSetKernelArg(sortMortonKernel , 1 , sizeof(uint), 


	//clEnqueueReadBuffer(queue,positionMem,CL_TRUE,0, sizeof(cl_float3) * objects.size(), &position[0],0,0,0);
	//clReleaseMemObject(positionMem);
	float wx =box.max[0] - box.min[0];
	float wy =box.max[1] - box.min[1];
	float wz =box.max[2] - box.min[2];

	float s = pow((wx * wy * wz)/objects.size(), 0.3333333);

	nx = multi * wx / s + 1;
	ny = multi * wy / s + 1;
	nz = multi * wz / s + 1;

	numCells = nx * ny * nz;
	numCells = numCells;
	numberOfCellObjects = 0;
	vector<cl_uint> cells;
	cells.resize(numCells);
	vector<BBox> cBoxes;
	cBoxes.resize(numCells);
	initCellWorkSize[1] = numCells;
	cellsMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int) * numCells, &cells[0],&err);
	cellsBoxMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(BBox) * numCells, &cBoxes[0],&err);
	sumMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int), &numberOfCellObjects,&err);

	//vector<Octree> nodes;
	//nodes.resize(treeManager->octreeNodes.size());
	//cl_mem treeMem = clCreateBuffer(context,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(Octree) * treeManager->octreeNodes.size(), &treeManager->octreeNodes[0],&err);;
	//cl_mem treeAccessMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(Octree) * treeManager->octreeNodes.size(), &nodes[0],&err);;
	//int sizeoftree = treeManager->octreeNodes.size();
	err |= clSetKernelArg(initializeCellsKernel,0 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(initializeCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(initializeCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	err |= clSetKernelArg(initializeCellsKernel,3 , sizeof(cl_int), &nx);
	err |= clSetKernelArg(initializeCellsKernel,4 , sizeof(cl_int), &ny);
	err |= clSetKernelArg(initializeCellsKernel,5 , sizeof(cl_int), &nz);
	err |= clSetKernelArg(initializeCellsKernel, 6, sizeof(cl_mem),&cellsBoxMem );
	err |= clSetKernelArg(initializeCellsKernel, 7, sizeof(cl_mem), &sumMem );
	//err |= clSetKernelArg(initializeCellsKernel, 8, sizeof(cl_mem), &treeMem );
	//err |= clSetKernelArg(initializeCellsKernel, 9, sizeof(cl_mem), &treeAccessMem );
	//err |= clSetKernelArg(initializeCellsKernel, 10, sizeof(cl_int), &sizeoftree);




	//err |= clSetKernelArg(initializeCellsKernel, 7, sizeof(cl_mem),&sumMem);

	if(err != CL_SUCCESS) {
		perror("Couldn't set the initializeCellsKernel argument");
		exit(1);   
	}

	err = clEnqueueNDRangeKernel(queue, initializeCellsKernel, 2 ,
		NULL, initCellWorkSize, 
		NULL, 0, NULL, NULL);
	//int theS;
	clEnqueueReadBuffer(queue,cellsMem,CL_TRUE,0, sizeof(int) * numCells, &cells[0],0,0,0);
	clEnqueueReadBuffer(queue,cellsBoxMem,CL_TRUE,0, sizeof(BBox) * numCells, &cBoxes[0],0,0,0);
	clEnqueueReadBuffer(queue,sumMem,CL_TRUE,0, sizeof(cl_int), &numberOfCellObjects,0,0,0);
	//clEnqueueReadBuffer(queue,treeAccessMem,CL_TRUE,0, sizeof(Octree) * treeManager->octreeNodes.size(), &nodes[0],0,0,0);

	vector<int> cellIndices;
	cellIndices.resize(numCells);

	//timer.start();
	cellIndices[0] = cells[0]; 
	for (int i = 1; i < numCells; i++)
	{
		cellIndices[i] = cells[i] + cellIndices[i-1];
	}
	//cout << timer.elapsed() << endl;

	//vector<cl_uint> cellsTest;
	//cellsTest.resize(numCells);
	//for (int i = 0; i < objects.size(); i++)
	//{
	//	for (int j = 0; j < numCells; j++)
	//	{
	//		if(bboxCollided(objects[i].box, cBoxes[j]))
	//		cellsTest[j]++;
	//	}
	//}


	cellIncrements.resize(numCells);
	objectIndices.resize(numberOfCellObjects);


	cellIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, cellIndices.size() * sizeof(cl_int),&cellIndices[0], &err);
	objectIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * numberOfCellObjects, &objectIndices[0], &err );
	cellIncrementsMem = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR, sizeof(cl_int)* numCells, &cellIncrements[0], &err);

}
void RenderWindow::initializeCellObjects()
{
	err |= clSetKernelArg(findObjectCellsKernel,0 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(findObjectCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(findObjectCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	err |= clSetKernelArg(findObjectCellsKernel,3 , sizeof(cl_mem) , &cellIndicesMem);
	err |= clSetKernelArg(findObjectCellsKernel,4 , sizeof(cl_mem)  , &objectIndicesMem);
	err |= clSetKernelArg(findObjectCellsKernel,5 , sizeof(cl_mem) , &cellsBoxMem);
	err |= clSetKernelArg(findObjectCellsKernel,6 , sizeof(cl_mem) , &cellIncrementsMem);

	if(err != CL_SUCCESS) {
		perror("Couldn't set the findObjectCells argument");
		exit(1);   
	} 

	err = clEnqueueNDRangeKernel(queue, findObjectCellsKernel, 2 ,
		NULL, initCellWorkSize, 
		NULL, 0, NULL, NULL);

	clEnqueueReadBuffer(queue,objectIndicesMem,CL_TRUE,0, sizeof(int) * numberOfCellObjects, &objectIndices[0],0,0,0);
	clEnqueueReadBuffer(queue,cellIncrementsMem,CL_TRUE,0,sizeof(int) * numCells, &cellIncrements[0],0,0,0);

}
void RenderWindow::initializeDrawScene()
{

	err |= clSetKernelArg(drawSceneKernel,0 , sizeof(cl_mem), &clImage);
	err |= clSetKernelArg(drawSceneKernel,1 , sizeof(cl_mem), &writeCLImage);
	err |= clSetKernelArg(drawSceneKernel,2 , sizeof(cl_sampler), &sampler);
	err |= clSetKernelArg(drawSceneKernel,3 , sizeof(cl_int), &windowWidth);
	err |= clSetKernelArg(drawSceneKernel,4 , sizeof(cl_int), &windowHeight);
	err |= clSetKernelArg(drawSceneKernel,5 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(drawSceneKernel,6 , sizeof(cl_mem), &lightMem);
	err |= clSetKernelArg(drawSceneKernel,7 , sizeof(cl_int), &numberOfObjects);
	err |= clSetKernelArg(drawSceneKernel,8 , sizeof(cl_int), &numberOfLights);
	err |= clSetKernelArg(drawSceneKernel,9, sizeof(BBox), &box);
	err |= clSetKernelArg(drawSceneKernel, 10, sizeof(cl_mem), &cellsMem);
	err |= clSetKernelArg(drawSceneKernel,11 , sizeof(cl_int), &nx);
	err |= clSetKernelArg(drawSceneKernel,12 , sizeof(cl_int), &ny);
	err |= clSetKernelArg(drawSceneKernel,13 , sizeof(cl_int), &nz);
	err |= clSetKernelArg(drawSceneKernel, 14, sizeof(cl_mem), &cellIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 15, sizeof(cl_mem), &objectIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 16, sizeof(Camera), &camera);



	if(err != CL_SUCCESS) {
		perror("Couldn't set the drawSceneKernel argument");
		exit(1);   
	}         
	globalWorkSize[0] = windowWidth;
	globalWorkSize[1] =  windowHeight;


	err = clEnqueueNDRangeKernel(queue, drawSceneKernel, 2 ,
		NULL, globalWorkSize, 
		NULL, 0, NULL, NULL);



	if(err < 0) {
		perror("Couldn't enqueue the drawSceneKernel execution command");
		exit(1);   
	}

	readBuffer = new uchar [windowWidth*windowHeight * 4];



	if(err != CL_SUCCESS)
	{
		cout << "Error creating Object" << endl;
		exit(-1);
	}


	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	region[0] = windowWidth;
	region[1] = windowHeight;
	region[2] = 1;
	err = clEnqueueReadImage(queue,writeCLImage,CL_TRUE,origin,region,0,0,readBuffer,0,NULL,NULL);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating reading from buffer" << endl;
		exit(-1);
	}
}
void RenderWindow::initializeCL()
{

	initializeProgram();
	initializeMemory();
	profileTimer.start();
	initializeSceneBBox();
	float time = (profileTimer.elapsed()/1000.0f);
	cout << "SceneBox : " << time  << endl;

	timer.start();
	time = timer.elapsed()/ 1000.0f;

	profileTimer.start();
	initializeCells();
	cout << "Initialize Cells : " << (profileTimer.elapsed()/1000.0f) << endl;
	profileTimer.start();

	//delete treeManager;
	cout << "Done" << endl;

	initializeCellObjects();
	cout << "Initialize Cell Objects : " << (profileTimer.elapsed()/1000.0f) << endl;
	profileTimer.start();
	initializeDrawScene();
	cout << "DrawScene : " << (profileTimer.elapsed()/1000.0f) << endl;
	cout << "Done!" << endl;
}
