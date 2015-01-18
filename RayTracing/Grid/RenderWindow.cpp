#include "RenderWindow.h"

/*
CL_DEVICE_MAX_WORK_ITEM_SIZES - 1024, 1024, 64
*/

/*index of refraction 
Acrylic glass - 1.490
glass in general seems to be 1.4 to 2.0
diamond 2.4

*/

const cl_float RenderWindow::MIN = 10000000.0f;
const cl_float RenderWindow::MAX = -10000000.0f;
const cl_uint RenderWindow::NUMBER_OF_SPHERES = 1;


RenderWindow::RenderWindow(void) : multi(2.0f),camera(glm::vec3(0.0f,0,20.0f), glm::vec3(0,0,0)) , random(Random::getInstance()), shadowsEnabled(false), 
	numberOfReflections(1), reflectionsEnabled(false), refractionsEnabled(false) , numberOfRefractions(1),
	globalIllumination(false), randomInt(0), initialized(false) , maxDepth(1),backgroundColor(.0,.0,.0,1), currentTime(0.0f), numberOfMeshes(0)
{
	camera.type = CameraType::Thinlens;
	BBox b = {glm::vec3(MIN,MIN,MIN),0.0f,glm::vec3(MAX,MAX,MAX),0.0f};
	box = b;
	setSamples(1);

	initializeProgram();
	construct();

	initialized = true;
	setMouseTracking(true);

}

void RenderWindow::setMaxBounce(int numBounces)
{
	maxDepth = numBounces;
}

bool RenderWindow::globalIlluminationEnabled()
{
	return globalIllumination;
}
void RenderWindow::setGlobalIllumination(bool enabled)
{
	globalIllumination = enabled;
}

void RenderWindow::changeLightType(int index)
{
	lights[0].type = (LightType)index;
}

void RenderWindow::changeCameraType(int index)
{
	camera.type = (CameraType)index;
}

void RenderWindow::changeLightType( QString index)
{
	//lights[0].type = (LightType)index;
}

RenderWindow::~RenderWindow(void)
{


	delete[]readBuffer;
	clReleaseMemObject(clImage);
	clReleaseMemObject(octreeMem);
	clReleaseMemObject(kdTreeMem);
	clReleaseMemObject(kdTreeIndicesMem);
	//clReleaseMemObject(objectPhotonCount);

	clReleaseMemObject(writeCLImage);
	clReleaseSampler(sampler);
	clReleaseKernel(drawSceneKernel);
	clReleaseKernel(drawShadowRaysKernel);
	clReleaseKernel(drawReflectionRaysKernel);
	clReleaseKernel(drawRefractionRaysKernel);
	clReleaseKernel(sceneBBoxKernel);
	clReleaseKernel(initializeCellsKernel);
	clReleaseKernel(findObjectCellsKernel);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseContext(context);
	releaseUpdate();

}
bool RenderWindow::isShadowsEnabled()const
{
	return shadowsEnabled;
}
bool RenderWindow::isReflectionsEnabled()const
{
	return reflectionsEnabled;
}
bool RenderWindow::isRefractionsEnabled()const
{
	return refractionsEnabled;
}


void RenderWindow::drawSecondaryRays()
{
	if(reflectionsEnabled)
	{
		setUpReflectionArgs();

		err = clEnqueueNDRangeKernel(queue, drawReflectionRaysKernel, 2 ,
			NULL, globalWorkSize, 
			NULL, 0, NULL, NULL);
	}
	if(refractionsEnabled)
	{
		setUpRefractionArgs();

		err = clEnqueueNDRangeKernel(queue, drawRefractionRaysKernel, 2 ,
			NULL, globalWorkSize, 
			NULL, 0, NULL, NULL);
	}

	if(shadowsEnabled)
	{

		setUpShadowArgs();

		err = clEnqueueNDRangeKernel(queue, drawShadowRaysKernel, 2 ,
			NULL, globalWorkSize, 
			NULL, 0, NULL, NULL);
	}



}

void RenderWindow::setSamples(int samplesSquared)
{
	this->sampleSquared =samplesSquared;
	this->samples = pow(sampleSquared,2.0f);

	//if(initialized)
	//{
	//	clReleaseMemObject(depthBuffer);
	//	depthBuffer = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(cl_float)*windowWidth * windowHeight * samples, NULL,&err);;
	//}

}
void RenderWindow::addMesh(std::string fileName, glm::vec3 position , MaterialType type)
{
	Assimp::Importer importer;
	const aiScene * scene = importer.ReadFile(fileName, flags);
	aiMesh * mesh = (scene->mMeshes[0]);


	GameObject  * object = new GameObject(fileName.c_str(), Type::Mesh_Type,numberOfMeshes++);
	gameObjectList.append(object);
	//GameObject *  planeObject  = new GameObject("Plane", Type::Mesh_Type,1);

	std::vector<cl_uint> modelIndices;
	std::vector<glm::vec3> modelVertices;
	modelVertices.reserve(mesh->mNumVertices);

	if (mesh->HasFaces())
	{
		int faceCount = mesh->mNumFaces;
		for (int i = 0; i < faceCount; i++)
		{
			aiFace * face = &mesh->mFaces[i];
			for (UINT j = 0; j < face->mNumIndices; j++)
			{
				modelIndices.push_back(face->mIndices[j]);
			}
		}
	}


	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		modelVertices.push_back(glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[i])));
	}

	if(triangles.size())
		clReleaseMemObject(trianglesMem);

	if(objects.size())
		clReleaseMemObject(objectMem);

	if(meshes.size())
		clReleaseMemObject(meshMem);


	//int triangleInitSize = triangles.size();
	//	triangles.reserve(modelIndices.size() / 3);
	//	objects.reserve(modelIndices.size() / 3);
	Random random = Random::getInstance();

	Material material = {/*{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}, 
						 {random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)},*/
		//{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0f,5.0f)}, 1.0f,1.49f, (MaterialType)(rand()%3)};
		{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0f,5.0f)}, 1.0f,1.49f, type};
	Mesh m = {material, {position.x,position.y,position.z, 0.0f},{triangles.size(),triangles.size() + modelIndices.size()/3 }};
	uint meshIndex = meshes.size();
	meshes.push_back(m);
	//glm::vec3 position(0,0,10);
	for (UINT i = 0; i < modelIndices.size(); i+=3)
	{
		Triangle tri;
		tri.v0 = glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i]]));
		tri.v1 = glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i + 1]]));
		tri.v2 = glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i + 2]]));

		Object o = createObjectFromTriangle(tri, position);
		tri.v0 = position + glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i]]));
		tri.v1 = position + glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i + 1]]));
		tri.v2 = position + glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mVertices[modelIndices[i + 2]]));

		if(mesh->HasNormals())
		{
			tri.normal = glm::vec3(*reinterpret_cast<glm::vec3*>(&mesh->mNormals[modelIndices[i]]));
		}

		o.triangleIndex = triangles.size();
		o.meshIndex = meshIndex;
		o.index = objects.size();
		//o.material = material;
		objects.push_back(o);
		triangles.push_back(tri);
		box.max = glm::max(box.max, tri.v0);
		box.max = glm::max(box.max, tri.v1);
		box.max = glm::max(box.max, tri.v2);


		box.min = glm::min(box.min, tri.v0);
		box.min = glm::min(box.min, tri.v1);
		box.min = glm::min(box.min, tri.v2);
	}


	trianglesMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Triangle)* triangles.size(), &triangles[0],&err);
	objectMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Object)* objects.size(), &objects[0],&err);
	meshMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Mesh)* meshes.size(), &meshes[0],&err);

}
void RenderWindow::resizeEvent(QResizeEvent * e)
{
	//windowWidth = width();
	//windowHeight = height();
	//delete [] readBuffer;
	//readBuffer = new uchar [width() * height() * 4];
}
void RenderWindow::addLight(Light light)
{
	lights.push_back(light);
	numberOfLights++;
}
void RenderWindow::addObject(std::string path)
{
	objectPaths.push_back(path);
}
void RenderWindow::addSphere()
{
	Sphere sphere = {{{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}}, 
		//	{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)},
		//{random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0),random.getRandomFloat(0.0,1.0)}},
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
	//object.material = sphere.material;
	object.box = b;
	//cl_int2 indices = {0,0};
	object.triangleIndex =-1;
	//object.index = objects.size();
	//object.position = sphere.position;


	objects.push_back(object);
}

void RenderWindow::keyPressEvent(QKeyEvent * e)
{
}

void RenderWindow::construct()
{
	setMinimumSize(640,480);

	layout = new QHBoxLayout();
	Light light = {{{0.925f,0.835f,0.102f}}, {2.0f,6.0f,18.0f}, {1.0f,1.0f,-1.0f,2.0f}, LightType::POINT_TYPE };

	 lightObject = new GameObject("Light", Type::Light_Type,0);
	gameObjectList.append(lightObject);


	//addMesh("FutureCar.obj", glm::vec3(0.0f,2.0f,9.0f), MaterialType::SPECULAR);
	//addMesh("shadowPlane.obj", glm::vec3(0.0f,0.0f,6.0f),DIFFUSE);

//	addMesh("shadowPlane.obj", glm::vec3(0.0f,0.0f,6.0f), DIFFUSE);
//	addMesh("Helicopter.obj", glm::vec3(0.0f,0.0f,6.0f), DIFFUSE);
	//addMesh("cylinder.obj", glm::vec3(0.0f,0.0f,10.0f), SPECULAR);
	//addMesh("suzy2.obj", glm::vec3(0.0f,0.0f,10.0f), SPECULAR);
	//addMesh("suzy.obj", glm::vec3(0.0f,0.0f,8.0f), SPECULAR);
	//	addMesh("Box.obj", glm::vec3(0.0f,2.0f,9.0f));
	//addMesh("isoSphere.obj", glm::vec3(0.0f,2.0f,9.0f), MaterialType::SPECULAR);
	//	addMesh("simpleTorus.obj", glm::vec3(0.0f,2.0f,9.0f), MaterialType::DIFFUSE);
	//	addMesh("suzy.obj", glm::vec3(0.0f,0.0f,6.0f));

	addMesh("basicCube.obj", glm::vec3(0.0f,0.0f,8.5f), SPECULAR);
	addMesh("shadowPlane.obj", glm::vec3(0.0f,0.0f,6.0f), DIFFUSE);

	cameraObject = new GameObject("Camera", Type::Camera_Type, 0 );
	gameObjectList.append(cameraObject);

	box.min -= 0.001f;
	box.max +=  0.001f;


	lights.push_back(light);
	numberOfObjects = objects.size();
	numberOfLights = lights.size();

	layout->addWidget(&imageLabel);
	setLayout(layout);

	windowWidth = width();
	windowHeight = height();
	readBuffer = new uchar [windowWidth * windowHeight * 4];


	Octree root(box.min + ((box.max - box.min)/ 2.0f),box,0);
	octManager = OctreeManager(root);
	//octManager.insert(objects,meshes);
	octreeMem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Octree) * octManager.octreeNodes.size(),&octManager.octreeNodes[0], &err );

	DEPTH_REACHED++;

	KDTreeNode kdRoot(box,0,0);
	kdManager = KDTreeManager(kdRoot);
	//kdManager.insert(objects,meshes);

	kdTreeMem =clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(KDTreeNode) * kdManager.nodes.size(),&kdManager.nodes[0], &err );
	kdManager.leafObjectIndices.push_back(1);
	kdTreeIndicesMem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * kdManager.leafObjectIndices.size(),&kdManager.leafObjectIndices[0],&err);
	//vector<int> indices;
	//indices.resize(kdManager.leafObjectIndices.size());



	initializeCL();
	//	clEnqueueReadBuffer(queue,kdTreeIndicesMem,CL_TRUE,0,sizeof(int) * kdManager.leafObjectIndices.size(),&indices[0], 0,0,0);



	QImage r(readBuffer,windowWidth,windowHeight,QImage::Format::Format_RGBA8888);
	readImage = r;
}
void RenderWindow::updateDrawScene()
{
	//clEnqueueUnmapMemObject(queue,writeCLImage,readBuffer,0,0,0);
	clReleaseMemObject(meshMem);
	meshMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Mesh)* meshes.size(), &meshes[0],&err);


	size_t gSize[3] = {windowWidth,windowHeight,sampleSquared};
	setUpDrawSceneArgs();
	err |= clEnqueueNDRangeKernel(queue, drawSceneKernel, 3,
		NULL, &gSize[0], 
		NULL, 0, NULL, NULL);

	err |= clEnqueueReadImage(queue,writeCLImage,CL_TRUE,origin,region,0,0,readBuffer,0,NULL,NULL);


	/*vector<float> numbers;
	numbers.resize(windowWidth * windowHeight * samples);
	err |= clEnqueueReadBuffer(queue,depthBuffer,CL_TRUE,0, sizeof(float)*windowWidth * windowHeight * samples, &numbers[0],0,0,0);;
	*/


	//cl_uint rowPitch = 0;
	//readBuffer = (uchar*)clEnqueueMapImage(queue,writeCLImage,CL_TRUE,CL_MAP_READ,origin,region,&rowPitch,0,0,0,0, &err);
	if(err != CL_SUCCESS)
	{
		cout<< "Error reading buffer" << endl;
		exit(1);
	}


	//clEnqueueUnmapMemObject(queue,
	//qDebug() << err;
}

void RenderWindow::clearScene()
{
	BBox b = {glm::vec3(MIN,MIN,MIN),0.0f,glm::vec3(MAX,MAX,MAX),0.0f};
	box = b;
	numberOfMeshes = 0;

	initializeGameObjectList();
}


void RenderWindow::sceneInitialize()
{
	box.min -= 0.001f;
	box.max +=  0.001f;

	objectMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Object)* objects.size(), &objects[0],&err);
	if(err != CL_SUCCESS)
	{
		cout << "Error creating Object" << endl;
		exit(-1);
	}
	if(triangles.size() > 0)
	{

		trianglesMem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Triangle) * triangles.size(), &triangles[0], &err);

		if(err != CL_SUCCESS)
		{
			cout << "Error creating triangle Object" << endl;
			exit(-1);
		}
	}

	
	boundingBoxMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,sizeof(BBox), &box,&err);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating bounding box memory object" << endl;
		//cleanup
		exit(-1);
	}

	
	profileTimer.start();

	float times[5];

	initializeSceneBBox();

	//clEnqueueReadBuffer(queue,octreeMem,CL_TRUE, 0,sizeof(Octree) * octManager.octreeNodes.size(),&octManager.octreeNodes[0],0,0,0);//(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Octree) * octManager.octreeNodes.size(),&octManager.octreeNodes[0], &err );


	times[0] =  (profileTimer.elapsed()/1000.0f);
	profileTimer.start();
	initializeCells();
	times[1] =  (profileTimer.elapsed()/1000.0f);

	profileTimer.start();
	initializeCellObjects();
	times[2] =  (profileTimer.elapsed()/1000.0f);

	profileTimer.start();
	initializeDrawScene();
	times[3] =  (profileTimer.elapsed()/1000.0f);

	times[4] = timer.elapsed() / 1000.0f;


	cout << "Init SceneBox : " << times[0] << endl;
	cout << "Init Cells : " << times[1] << endl;
	cout << "Init Cell Objects : " << times[2] << endl;
	cout << "Init DrawScene : " << times[3] << endl;
	cout << "Init total : " << times[4] << endl;

	emit updateList();

}

void RenderWindow::initializeGameObjectList()
{
	gameObjectList.clear();
	gameObjectList.append(lightObject);
	gameObjectList.append(cameraObject);
}
void RenderWindow::sceneRelease()
{
	releaseUpdate();
	triangles.clear();
	objects.clear();
	meshes.clear();
}
void RenderWindow::scene1()
{
	sceneRelease();
	clearScene();
	addMesh("basicCube.obj", glm::vec3(0.0f,0.0f,8.5f), TRANS);
	addMesh("shadowPlane.obj", glm::vec3(0.0f,0.0f,6.0f), DIFFUSE);
	sceneInitialize();

}
void RenderWindow::scene2()
{
	sceneRelease();
	clearScene();
	addMesh("shadowPlane.obj", glm::vec3(0,0,6), SPECULAR);
	addMesh("cylinder.obj", glm::vec3(0.0f,0.0f,10.0f), SPECULAR);
	sceneInitialize();

}
void RenderWindow::scene3()
{
	sceneRelease();
	clearScene();
	addMesh("shadowPlane.obj", glm::vec3(0,0,6), SPECULAR);
	addMesh("smallSuzy.obj", glm::vec3(0.0f,1.0f,8.0f), DIFFUSE);

	sceneInitialize();

}
void RenderWindow::scene4()
{
	sceneRelease();
	clearScene();
	addMesh("suzy2.obj", glm::vec3(0.0f,0.0f,10.0f), SPECULAR);

	sceneInitialize();


}

void RenderWindow::scene5()
{
	sceneRelease();
	clearScene();
	addMesh("FutureCar.obj", glm::vec3(0,0,10), SPECULAR);

	sceneInitialize();

}


void RenderWindow::keyReleaseEvent(QKeyEvent * e)
{
	if (e->key() == Qt::Key_1)
	{
		scene1();
	}
	else if (e->key() == Qt::Key_2)
	{
		scene2();
	}
	else if (e->key() == Qt::Key_3)
	{
		scene3();
	}
	else if (e->key() == Qt::Key_4)
	{
		scene4();
	}
	else if (e->key() == Qt::Key_5)
	{
		scene5();
	}


}

void RenderWindow::updateScene()
{
	//releaseUpdate();

	//randomInt = rand() %100000 + 10000;
	this->samples = pow(sampleSquared, 2.0f);

	timer.start();	
	handleKeyInput();
	camera.update();

	// 0
	profileTimer.start();

	//updateBBox();
	//updateCells();
	randomInt = time;

	randSeed = QDateTime::currentMSecsSinceEpoch();

	updateDrawScene();


	// -1 to 1 and I have 0 to 1 0
	/*
	int photonSeed = rand() % 342242511;
	int photonsPerObject = 5000;
	int numberOfBounces = 3;
	size_t numberOfPhotons = 1000000;
	size_t numObj = objects.size();
	// objectPhotonCount = clCreateBuffer(context,CL_MEM_READ_WRITE, sizeof(cl_int) * objects.size() * 5000,NULL, &err);
	err |= clSetKernelArg(clearPhotonKernel,0,sizeof(cl_mem),&objectPhotonCount);

	err |= clEnqueueNDRangeKernel(queue, clearPhotonKernel, 1,
	NULL, &numObj, 
	NULL, 0, NULL, NULL);

	err |= clSetKernelArg(emitPhotonKernel,0 , sizeof(int), &photonSeed);
	err |= clSetKernelArg(emitPhotonKernel,1 , sizeof(int), &photonsPerObject);
	err |= clSetKernelArg(emitPhotonKernel,2 , sizeof(cl_mem), &objectPhotonCount);
	err |= clSetKernelArg(emitPhotonKernel,3 , sizeof(cl_mem), &lightMem);
	err |= clSetKernelArg(emitPhotonKernel,4 , sizeof(int), &numberOfBounces);
	err |= clSetKernelArg(emitPhotonKernel,5 , sizeof(BBox), &box);
	err |= clSetKernelArg(emitPhotonKernel,6 , sizeof(cl_mem), &cellIndicesMem);
	err |= clSetKernelArg(emitPhotonKernel,7 , sizeof(cl_mem), &objectIndicesMem);
	err |= clSetKernelArg(emitPhotonKernel,8 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(emitPhotonKernel,9 , sizeof(cl_mem), &writeCLImage);
	err |= clSetKernelArg(emitPhotonKernel,10 , sizeof(cl_float3), &voxelWidth[0]);
	err |= clSetKernelArg(emitPhotonKernel,11 , sizeof(cl_float3), &numberOfVoxels[0]);
	err |= clSetKernelArg(emitPhotonKernel,12 , sizeof(cl_int), &windowWidth);
	err |= clSetKernelArg(emitPhotonKernel,13 , sizeof(cl_int), &windowHeight);
	err |= clSetKernelArg(emitPhotonKernel,14 , sizeof(Camera), &camera);
	err |= clSetKernelArg(emitPhotonKernel,15 , sizeof(cl_mem), &trianglesMem);

	err |= clEnqueueNDRangeKernel(queue, emitPhotonKernel, 1,
	NULL, &numberOfPhotons, 
	NULL, 0, NULL, NULL);

	err |= clEnqueueReadImage(queue,writeCLImage,CL_TRUE,origin,region,0,0,readBuffer,0,NULL,NULL);
	*/


	imageLabel.setPixmap(QPixmap::fromImage(readImage));
	float drawTimer = profileTimer.elapsed()/1000.0f;
	long mili = timer.elapsed();
	interval = (mili/1000.0f);
	fps = 1.0f/interval;

	//randomInt += (int)mili;


	//	Light light = {{{0.925f,0.835f,0.102f}, {0.73f,0.724f,0.934f},{0.2f,0.52f,0.96f}}, {0.0f,0.0f,20.0f}, {1.0f,1.0f,-1.0f,2.0f}, LightType::DIRECTIONAL_TYPE };
	//	lights[0]  = light;
	clReleaseMemObject(lightMem);
	lightMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Light), &lights[0],&err);
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

	sceneBBoxGlobalWorkSize = objects.size();
	err |= clEnqueueNDRangeKernel(queue, sceneBBoxKernel, 1 ,
		NULL, &sceneBBoxGlobalWorkSize, 
		NULL, 0, NULL, NULL);

	err |=clEnqueueReadBuffer(queue,boundingBoxMem,CL_TRUE,0, sizeof(BBox), &box,0,0,0);



}
void RenderWindow::updateCells()
{
	initCellWorkSize = objects.size();
	calculateVoxelSize();



	cells.resize(totalVoxels);
	numberOfCellObjects = 0;
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


	clEnqueueReadBuffer(queue,objectIndicesMem,CL_TRUE,0, sizeof(int) * numberOfCellObjects, &objectIndices[0],0,0,0);
}
float RenderWindow::getFPS()
{
	return fps;
}
float RenderWindow::getInterval()
{
	return interval;
}
void RenderWindow::releaseDrawScene()
{
	if(triangles.size())clReleaseMemObject(trianglesMem);
	if(objects.size())clReleaseMemObject(objectMem);//
	//if(lights.size())clReleaseMemObject(lightMem);//
}
void RenderWindow::releaseCells()
{
	clReleaseMemObject(sumMem);
	clReleaseMemObject(cellsMem);//
	clReleaseMemObject(objectIndicesMem);//
	clReleaseMemObject(cellIndicesMem);//
	clReleaseMemObject(cellIncrementsMem);//

}
void RenderWindow::releaseBBox()
{
	clReleaseMemObject(boundingBoxMem);
	clReleaseMemObject(minMem);
	clReleaseMemObject(maxMem);
}
void RenderWindow::releaseUpdate()
{

	if(objectPaths.size() > 0)
	{
		for (uint i = 0; i < objectPaths.size(); i++)
		{
			addMesh(objectPaths[i]);
		}

		objectPaths.clear();
	}
	cellIndices.clear();
	objectIndices.clear();
	cellIncrements.clear();
	minArr.clear();
	maxArr.clear();
	cells.clear();

	releaseBBox();
	releaseCells();
	releaseDrawScene();


	//clReleaseMemObject(minMem);//
	//clReleaseMemObject(maxMem);//
	//if(triangles.size())
	//{
	//	clReleaseMemObject(trianglesMem);//
	//}

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
	const char options[] = "-cl-std=CL1.1 -cl-mad-enable -Werror -cl-nv-verbose";

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
	std::string files []  = { "Kernels/math.opencl", "Kernels/random.opencl", "Kernels/structs.opencl","Kernels/kernelHelperFunctions.opencl","Kernels/drawScene.opencl", "Kernels/generateSceneBBox.opencl","Kernels/initializeCells.opencl","Kernels/findObjectCells.opencl",  "Kernels/photonMapping.opencl"};
	program = buildProgram(files, 9);
	drawSceneKernel = clCreateKernel(program, "drawScene", &err);
	drawShadowRaysKernel = clCreateKernel(program, "drawShadowRays", &err);
	drawReflectionRaysKernel = clCreateKernel(program, "drawReflectionRays", &err);
	drawRefractionRaysKernel = clCreateKernel(program, "drawRefractionRays", &err);
	sceneBBoxKernel = clCreateKernel(program, "generateSceneBBox", &err);
	initializeCellsKernel = clCreateKernel(program, "initializeCells", &err);
	findObjectCellsKernel = clCreateKernel(program,"findObjectCells", &err);
	emitPhotonKernel = clCreateKernel(program,"emitPhotons", &err);
	clearPhotonKernel = clCreateKernel(program,"clearPhotons", &err);
}
void RenderWindow::initializeSceneBBox()
{
	numberOfObjects = objects.size();
	int globalObjectSize = nextPowerOfTwo(objects.size());

	minArr.resize(globalObjectSize);
	maxArr.resize(globalObjectSize);

	minMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &minArr[0],&err);
	maxMem = clCreateBuffer(context, CL_MEM_USE_HOST_PTR, sizeof(cl_float3) * globalObjectSize, &maxArr[0],&err);
	//finish setting up mem stuff in header file...

	setUpSceneBoxArgs();

	if(err != CL_SUCCESS) {
		perror("Couldn't set the sceneBBoxKernel argument");
		exit(1);   
	} 


	queue = clCreateCommandQueue(context, device, 0, &err);
	if(err < 0) {
		perror("Couldn't create the command queue");
		exit(1);   
	}


	//sceneBBoxGlobalWorkSize = nextPowerOfTwo(objects.size());
	//err = clEnqueueNDRangeKernel(queue, sceneBBoxKernel, 1 ,
	//	NULL, &sceneBBoxGlobalWorkSize, 
	//	NULL, 0, NULL, NULL);

	//err |= clEnqueueReadBuffer(queue,boundingBoxMem,CL_TRUE,0, sizeof(BBox), &box,0,0,0);
}
void RenderWindow::initializeCells()
{
	QElapsedTimer t;
	t.start();
	initCellWorkSize = objects.size();

	calculateVoxelSize();

	numberOfCellObjects = 0;
	cells.resize(totalVoxels);
	cellsMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int) * totalVoxels, &cells[0],&err);
	sumMem = clCreateBuffer(context,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR ,sizeof(int), &numberOfCellObjects,&err);


	setUpCellArgs();

	if(err != CL_SUCCESS) {
		perror("Couldn't set the initializeCellsKernel argument");
		exit(1);   
	}

	err = clEnqueueNDRangeKernel(queue, initializeCellsKernel, 1 ,
		NULL, &initCellWorkSize, 
		NULL, 0, NULL, NULL);
	vector<Mesh> og;
	og.resize(meshes.size());

	err |= clEnqueueReadBuffer(queue,meshMem,CL_TRUE,0, sizeof(Mesh) * meshes.size(), &og[0],0,0,0);


	err |= clEnqueueReadBuffer(queue,cellsMem,CL_TRUE,0, sizeof(int) * totalVoxels, &cells[0],0,0,0);
	err |= clEnqueueReadBuffer(queue,sumMem,CL_TRUE,0, sizeof(cl_int), &numberOfCellObjects,0,0,0);


	cellIndices.resize(totalVoxels);

	cellIndices[0] = cells[0]; 


	for (int i = 1; i < totalVoxels; i++)
	{
		cellIndices[i] = cells[i] + cellIndices[i-1];
	}

	cellIncrements.resize(totalVoxels);
	objectIndices.resize(numberOfCellObjects);


	//for(int i = 0; i < objects.size(); i++)
	//{
	//	glm::vec3 cellMin = positionToVoxel(objects[i].box.min  + glm::vec3(objects[i].position),voxelInvWidth, numberOfVoxels,box );
	//	glm::vec3 cellMax = positionToVoxel(objects[i].box.max + glm::vec3(objects[i].position),voxelInvWidth, numberOfVoxels,box );

	//	for(int z = cellMin.z; z <= cellMax.z; z++)
	//	{
	//		for(int y = cellMin.y; y <= cellMax.y; y++)
	//		{
	//			for(int x = cellMin.x; x <= cellMax.x; x++)
	//			{
	//				cells[findVoxelIndex(glm::vec3(x,y,z),numberOfVoxels)]++;
	//				totalVoxels++;
	//			}
	//		}
	//	}
	//}

	cellIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR | CL_MEM_READ_ONLY, cellIndices.size() * sizeof(cl_int),&cellIndices[0], &err);
	objectIndicesMem = clCreateBuffer(context,CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * numberOfCellObjects, &objectIndices[0], &err );
	cellIncrementsMem = clCreateBuffer(context, CL_MEM_COPY_HOST_PTR, sizeof(cl_int)* totalVoxels, &cellIncrements[0], &err);
	long result = t.elapsed();

}
void RenderWindow::initializeCellObjects()
{
	setUpCellObjectArgs();

	if(err != CL_SUCCESS) {
		perror("Couldn't set the findObjectCells argument");
		exit(1);   
	} 

	err = clEnqueueNDRangeKernel(queue, findObjectCellsKernel, 1 ,
		NULL, &initCellWorkSize, 
		NULL, 0, NULL, NULL);
}
void RenderWindow::initializeDrawScene()
{

	setUpDrawSceneArgs();



	if(err != CL_SUCCESS) {
		perror("Couldn't set the drawSceneKernel argument");
		exit(1);   
	}         
	globalWorkSize[0] = windowWidth;
	globalWorkSize[1] =  windowHeight;

	err = 0;
	err = clEnqueueNDRangeKernel(queue, drawSceneKernel, 2 ,
		NULL, globalWorkSize, 
		NULL, 0, NULL, NULL);



	if(err < 0) {
		perror("Couldn't enqueue the drawSceneKernel execution command");
		exit(1);   
	}

	//drawSecondaryRays();

	origin[0] = 0;
	origin[1] = 0;
	origin[2] = 0;
	region[0] = windowWidth;
	region[1] = windowHeight;
	region[2] = 1;
	err = clEnqueueReadImage(queue,writeCLImage,CL_TRUE,origin,region,0,0,readBuffer,0,NULL,NULL);
	//cl_uint rowPitch = 0;
	//readBuffer = (uchar*)clEnqueueMapImage(queue,writeCLImage,CL_TRUE,CL_MAP_READ,origin,region,&rowPitch,0,0,0,0, &err);

	if(err != CL_SUCCESS)
	{
		cout << "Error creating reading from buffer" << endl;
		exit(-1);
	}

}
void RenderWindow::initializeMemory()
{
	cl_image_format clImageFormat;
	clImageFormat.image_channel_order = CL_RGBA;
	clImageFormat.image_channel_data_type = CL_UNSIGNED_INT8;
	//cl_image_format depthBufferFormat;
	//depthBufferFormat.image_channel_order = CL_R;
	//depthBufferFormat.image_channel_data_type = CL_FLOAT;

	objectMem = clCreateBuffer(context,CL_MEM_READ_ONLY  | CL_MEM_COPY_HOST_PTR,sizeof(Object)* objects.size(), &objects[0],&err);
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

	if(triangles.size() > 0)
	{

		trianglesMem = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Triangle) * triangles.size(), &triangles[0], &err);

		if(err != CL_SUCCESS)
		{
			cout << "Error creating triangle Object" << endl;
			exit(-1);
		}
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


	//depthBuffer = clCreateImage2D(context,CL_MEM_READ_WRITE, &depthBufferFormat, windowWidth,windowHeight, 0 ,NULL, &err);
	depthBuffer = clCreateBuffer(context,CL_MEM_READ_WRITE,sizeof(cl_float)*windowWidth * windowHeight * samples, NULL,&err);;

	//if(err != CL_SUCCESS)
	//{
	//	cout << "Error creating depthBuffer CL Image object : " << err << endl;
	//	exit(-1);
	//}


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
		cout << "Error creating bounding box memory object" << endl;
		//cleanup
		exit(-1);
	}
}


void RenderWindow::setShadowsEnabled(bool enabled )
{
	shadowsEnabled = enabled;
}

void RenderWindow::setReflectionsEnabled(bool enabled)
{
	reflectionsEnabled = enabled;
}
void RenderWindow::setRefractionsEnabled(bool enabled)
{
	refractionsEnabled = enabled;
}

void RenderWindow::initializeCL()
{

	//initializeProgram();
	initializeMemory();

	//objectPhotonCount = clCreateBuffer(context,CL_MEM_READ_WRITE, sizeof(cl_int) * objects.size(),NULL, &err);

	profileTimer.start();
	timer.start();

	float times[5];

	initializeSceneBBox();

	//clEnqueueReadBuffer(queue,octreeMem,CL_TRUE, 0,sizeof(Octree) * octManager.octreeNodes.size(),&octManager.octreeNodes[0],0,0,0);//(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(Octree) * octManager.octreeNodes.size(),&octManager.octreeNodes[0], &err );


	times[0] =  (profileTimer.elapsed()/1000.0f);
	profileTimer.start();
	initializeCells();
	times[1] =  (profileTimer.elapsed()/1000.0f);

	profileTimer.start();
	initializeCellObjects();
	times[2] =  (profileTimer.elapsed()/1000.0f);

	profileTimer.start();
	initializeDrawScene();
	times[3] =  (profileTimer.elapsed()/1000.0f);

	times[4] = timer.elapsed() / 1000.0f;


	cout << "Init SceneBox : " << times[0] << endl;
	cout << "Init Cells : " << times[1] << endl;
	cout << "Init Cell Objects : " << times[2] << endl;
	cout << "Init DrawScene : " << times[3] << endl;
	cout << "Init total : " << times[4] << endl;

}

void RenderWindow::setUpDrawSceneArgs()
{
	err |= clSetKernelArg(drawSceneKernel,0 , sizeof(cl_mem), &clImage);
	err |= clSetKernelArg(drawSceneKernel,1 , sizeof(cl_mem), &writeCLImage);
	err |= clSetKernelArg(drawSceneKernel,2 , sizeof(cl_mem), &depthBuffer);
	err |= clSetKernelArg(drawSceneKernel,3 , sizeof(cl_sampler), &sampler);
	err |= clSetKernelArg(drawSceneKernel,4 , sizeof(cl_int), &windowWidth);
	err |= clSetKernelArg(drawSceneKernel,5 , sizeof(cl_int), &windowHeight);
	err |= clSetKernelArg(drawSceneKernel,6 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(drawSceneKernel,7 , sizeof(cl_mem),(triangles.size())? &trianglesMem : nullptr);
	err |= clSetKernelArg(drawSceneKernel,8 , sizeof(cl_mem), &lightMem);
	err |= clSetKernelArg(drawSceneKernel,9 , sizeof(cl_int), &numberOfObjects);
	err |= clSetKernelArg(drawSceneKernel,10 , sizeof(cl_int), &numberOfLights);
	err |= clSetKernelArg(drawSceneKernel,11, sizeof(BBox), &box);
	err |= clSetKernelArg(drawSceneKernel, 12, sizeof(cl_mem), &cellsMem);
	err |= clSetKernelArg(drawSceneKernel,13 , sizeof(cl_float3), &numberOfVoxels[0]);
	err |= clSetKernelArg(drawSceneKernel, 14, sizeof(cl_mem), &cellIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 15, sizeof(cl_mem), &objectIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 16, sizeof(Camera), &camera);
	err |= clSetKernelArg(drawSceneKernel,17 , sizeof(cl_int), &samples);
	err |= clSetKernelArg(drawSceneKernel,18 , sizeof(cl_int), &sampleSquared);
	err |= clSetKernelArg(drawSceneKernel,19 , sizeof(cl_float3) , &delta[0]);
	err |= clSetKernelArg(drawSceneKernel,20 , sizeof(cl_float3) , &deltaInv[0]);
	err |= clSetKernelArg(drawSceneKernel,21 , sizeof(cl_float3) , &voxelInvWidth[0]);
	err |= clSetKernelArg(drawSceneKernel,22 , sizeof(cl_float3) , &numberOfVoxels[0]);
	err |= clSetKernelArg(drawSceneKernel,23 , sizeof(cl_float3) , &voxelWidth[0]);
	err |= clSetKernelArg(drawSceneKernel,24 , sizeof(cl_int) , &randomInt);
	err |= clSetKernelArg(drawSceneKernel,25 , sizeof(cl_int) , &maxDepth);
	err |= clSetKernelArg(drawSceneKernel,26 , sizeof(cl_int) , &shadowsEnabled);
	err |= clSetKernelArg(drawSceneKernel,27 , sizeof(cl_int) , &reflectionsEnabled);
	err |= clSetKernelArg(drawSceneKernel,28 , sizeof(cl_int) , &refractionsEnabled);
	err |= clSetKernelArg(drawSceneKernel, 29, sizeof(cl_float4), &backgroundColor);
	err |= clSetKernelArg(drawSceneKernel, 30, sizeof(cl_mem), &meshMem);
	err |= clSetKernelArg(drawSceneKernel, 31, sizeof(cl_mem), &kdTreeMem);
	err |= clSetKernelArg(drawSceneKernel, 32, sizeof(cl_mem), &kdTreeIndicesMem);
	err |= clSetKernelArg(drawSceneKernel, 33, sizeof(int) * nextPowerOfTwo(MAX_KD_DEPTH + 1), NULL);

	//err |= clSetKernelArg(drawSceneKernel, 33, sizeof(cl_mem), &octreeMem);
	//err |= clSetKernelArg(drawSceneKernel, 34, sizeof(float) * nextPowerOfTwo(DEPTH_REACHED), NULL);
	//err |= clSetKernelArg(drawSceneKernel, 35, sizeof(int) * nextPowerOfTwo(DEPTH_REACHED), NULL);
	//err |= clSetKernelArg(drawSceneKernel, 36, sizeof(int) , &DEPTH_REACHED);
}

void RenderWindow::setUpReflectionArgs()
{
	err |= clSetKernelArg(drawReflectionRaysKernel,0 , sizeof(cl_mem), &writeCLImage);
	err |= clSetKernelArg(drawReflectionRaysKernel,1 , sizeof(cl_mem), &depthBuffer);
	err |= clSetKernelArg(drawReflectionRaysKernel,2 , sizeof(cl_sampler), &sampler);
	err |= clSetKernelArg(drawReflectionRaysKernel,3 , sizeof(cl_int), &windowWidth);
	err |= clSetKernelArg(drawReflectionRaysKernel,4 , sizeof(cl_int), &windowHeight);
	err |= clSetKernelArg(drawReflectionRaysKernel,5 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(drawReflectionRaysKernel,6 , sizeof(cl_mem),(triangles.size())? &trianglesMem : nullptr);
	err |= clSetKernelArg(drawReflectionRaysKernel,7 , sizeof(cl_mem), &lightMem);
	err |= clSetKernelArg(drawReflectionRaysKernel,8 , sizeof(cl_int), &numberOfObjects);
	err |= clSetKernelArg(drawReflectionRaysKernel,9 , sizeof(cl_int), &numberOfLights);
	err |= clSetKernelArg(drawReflectionRaysKernel,10, sizeof(BBox), &box);
	err |= clSetKernelArg(drawReflectionRaysKernel, 11, sizeof(cl_mem), &cellsMem);
	err |= clSetKernelArg(drawReflectionRaysKernel,12 , sizeof(cl_float3), &numberOfVoxels[0]);
	err |= clSetKernelArg(drawReflectionRaysKernel, 13, sizeof(cl_mem), &cellIndicesMem);
	err |= clSetKernelArg(drawReflectionRaysKernel, 14, sizeof(cl_mem), &objectIndicesMem);
	err |= clSetKernelArg(drawReflectionRaysKernel, 15, sizeof(Camera), &camera);
	err |= clSetKernelArg(drawReflectionRaysKernel,16, sizeof(cl_int), &samples);
	err |= clSetKernelArg(drawReflectionRaysKernel,17 , sizeof(cl_int), &sampleSquared);
	err |= clSetKernelArg(drawReflectionRaysKernel,18 , sizeof(cl_float3) , &delta[0]);
	err |= clSetKernelArg(drawReflectionRaysKernel,19 , sizeof(cl_float3) , &deltaInv[0]);
	err |= clSetKernelArg(drawReflectionRaysKernel,20 , sizeof(cl_float3) , &voxelInvWidth[0]);
	err |= clSetKernelArg(drawReflectionRaysKernel,21 , sizeof(cl_float3) , &numberOfVoxels[0]);
	err |= clSetKernelArg(drawReflectionRaysKernel,22 , sizeof(cl_float3) , &voxelWidth[0]);
	err |= clSetKernelArg(drawReflectionRaysKernel,23 , sizeof(cl_int) , &numberOfReflections);
	err |= clSetKernelArg(drawReflectionRaysKernel,24 , sizeof(cl_int) , &randomInt);
}

void RenderWindow::setUpRefractionArgs()
{
	err |= clSetKernelArg(drawRefractionRaysKernel,0 , sizeof(cl_mem), &writeCLImage);
	err |= clSetKernelArg(drawRefractionRaysKernel,1 , sizeof(cl_mem), &depthBuffer);
	err |= clSetKernelArg(drawRefractionRaysKernel,2 , sizeof(cl_sampler), &sampler);
	err |= clSetKernelArg(drawRefractionRaysKernel,3 , sizeof(cl_int), &windowWidth);
	err |= clSetKernelArg(drawRefractionRaysKernel,4 , sizeof(cl_int), &windowHeight);
	err |= clSetKernelArg(drawRefractionRaysKernel,5 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(drawRefractionRaysKernel,6 , sizeof(cl_mem),(triangles.size())? &trianglesMem : nullptr);
	err |= clSetKernelArg(drawRefractionRaysKernel,7 , sizeof(cl_mem), &lightMem);
	err |= clSetKernelArg(drawRefractionRaysKernel,8 , sizeof(cl_int), &numberOfObjects);
	err |= clSetKernelArg(drawRefractionRaysKernel,9 , sizeof(cl_int), &numberOfLights);
	err |= clSetKernelArg(drawRefractionRaysKernel,10, sizeof(BBox), &box);
	err |= clSetKernelArg(drawRefractionRaysKernel, 11, sizeof(cl_mem), &cellsMem);
	err |= clSetKernelArg(drawRefractionRaysKernel,12 , sizeof(cl_float3), &numberOfVoxels[0]);
	err |= clSetKernelArg(drawRefractionRaysKernel, 13, sizeof(cl_mem), &cellIndicesMem);
	err |= clSetKernelArg(drawRefractionRaysKernel, 14, sizeof(cl_mem), &objectIndicesMem);
	err |= clSetKernelArg(drawRefractionRaysKernel, 15, sizeof(Camera), &camera);
	err |= clSetKernelArg(drawRefractionRaysKernel,16, sizeof(cl_int), &samples);
	err |= clSetKernelArg(drawRefractionRaysKernel,17 , sizeof(cl_int), &sampleSquared);
	err |= clSetKernelArg(drawRefractionRaysKernel,18 , sizeof(cl_float3) , &delta[0]);
	err |= clSetKernelArg(drawRefractionRaysKernel,19 , sizeof(cl_float3) , &deltaInv[0]);
	err |= clSetKernelArg(drawRefractionRaysKernel,20 , sizeof(cl_float3) , &voxelInvWidth[0]);
	err |= clSetKernelArg(drawRefractionRaysKernel,21 , sizeof(cl_float3) , &numberOfVoxels[0]);
	err |= clSetKernelArg(drawRefractionRaysKernel,22 , sizeof(cl_float3) , &voxelWidth[0]);
	err |= clSetKernelArg(drawRefractionRaysKernel,23 , sizeof(cl_int) , &numberOfReflections);
	err |= clSetKernelArg(drawRefractionRaysKernel,24 , sizeof(cl_int) , &randomInt);

}

void RenderWindow::setUpShadowArgs()
{
	err |= clSetKernelArg(drawShadowRaysKernel,0 , sizeof(cl_mem), &writeCLImage);
	err |= clSetKernelArg(drawShadowRaysKernel,1 , sizeof(cl_mem), &depthBuffer);
	err |= clSetKernelArg(drawShadowRaysKernel,2 , sizeof(cl_sampler), &sampler);
	err |= clSetKernelArg(drawShadowRaysKernel,3 , sizeof(cl_int), &windowWidth);
	err |= clSetKernelArg(drawShadowRaysKernel,4 , sizeof(cl_int), &windowHeight);
	err |= clSetKernelArg(drawShadowRaysKernel,5 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(drawShadowRaysKernel,6 , sizeof(cl_mem),(triangles.size())? &trianglesMem : nullptr);
	err |= clSetKernelArg(drawShadowRaysKernel,7 , sizeof(cl_mem), &lightMem);
	err |= clSetKernelArg(drawShadowRaysKernel,8 , sizeof(cl_int), &numberOfObjects);
	err |= clSetKernelArg(drawShadowRaysKernel,9 , sizeof(cl_int), &numberOfLights);
	err |= clSetKernelArg(drawShadowRaysKernel,10, sizeof(BBox), &box);
	err |= clSetKernelArg(drawShadowRaysKernel, 11, sizeof(cl_mem), &cellsMem);
	err |= clSetKernelArg(drawShadowRaysKernel,12 , sizeof(cl_float3), &numberOfVoxels[0]);
	err |= clSetKernelArg(drawShadowRaysKernel, 13, sizeof(cl_mem), &cellIndicesMem);
	err |= clSetKernelArg(drawShadowRaysKernel, 14, sizeof(cl_mem), &objectIndicesMem);
	err |= clSetKernelArg(drawShadowRaysKernel, 15, sizeof(Camera), &camera);
	err |= clSetKernelArg(drawShadowRaysKernel,16, sizeof(cl_int), &samples);
	err |= clSetKernelArg(drawShadowRaysKernel,17 , sizeof(cl_int), &sampleSquared);
	err |= clSetKernelArg(drawShadowRaysKernel,18 , sizeof(cl_float3) , &delta[0]);
	err |= clSetKernelArg(drawShadowRaysKernel,19 , sizeof(cl_float3) , &deltaInv[0]);
	err |= clSetKernelArg(drawShadowRaysKernel,20 , sizeof(cl_float3) , &voxelInvWidth[0]);
	err |= clSetKernelArg(drawShadowRaysKernel,21 , sizeof(cl_float3) , &numberOfVoxels[0]);
	err |= clSetKernelArg(drawShadowRaysKernel,22 , sizeof(cl_float3) , &voxelWidth[0]);
	err |= clSetKernelArg(drawShadowRaysKernel,23 , sizeof(cl_int) , &randomInt);


}

void RenderWindow::setUpCellArgs()
{
	err |= clSetKernelArg(initializeCellsKernel,0 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(initializeCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(initializeCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	err |= clSetKernelArg(initializeCellsKernel,3 , sizeof(cl_float3), &numberOfVoxels[0]); //checks out
	err |= clSetKernelArg(initializeCellsKernel, 4, sizeof(cl_float3),&voxelInvWidth[0] );
	err |= clSetKernelArg(initializeCellsKernel, 5, sizeof(cl_mem), &sumMem );
	err |= clSetKernelArg(initializeCellsKernel, 6, sizeof(cl_mem), &meshMem );
}
void RenderWindow::setUpCellObjectArgs()
{
	err |= clSetKernelArg(findObjectCellsKernel,0 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(findObjectCellsKernel,1 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(findObjectCellsKernel,2 , sizeof(cl_mem) , &cellsMem);
	err |= clSetKernelArg(findObjectCellsKernel,3 , sizeof(cl_mem) , &cellIndicesMem);
	err |= clSetKernelArg(findObjectCellsKernel,4 , sizeof(cl_mem)  , &objectIndicesMem);
	err |= clSetKernelArg(findObjectCellsKernel,5 , sizeof(cl_mem) , &cellIncrementsMem);
	err |= clSetKernelArg(findObjectCellsKernel,6 , sizeof(cl_float3) , &delta[0]);
	err |= clSetKernelArg(findObjectCellsKernel,7 , sizeof(cl_float3) , &deltaInv[0]);
	err |= clSetKernelArg(findObjectCellsKernel,8 , sizeof(cl_float3) , &voxelInvWidth[0]);
	err |= clSetKernelArg(findObjectCellsKernel,9 , sizeof(cl_float3) , &numberOfVoxels[0]); // checks out
	err |= clSetKernelArg(findObjectCellsKernel,10 , sizeof(cl_mem) ,&meshMem); // checks out
}
void RenderWindow::setUpSceneBoxArgs()
{
	err |= clSetKernelArg(sceneBBoxKernel,0 , sizeof(cl_mem), &boundingBoxMem);
	err |= clSetKernelArg(sceneBBoxKernel,1 , sizeof(cl_mem), &objectMem);
	err |= clSetKernelArg(sceneBBoxKernel,2 , sizeof(cl_int), &numberOfObjects);
	err |= clSetKernelArg(sceneBBoxKernel,3 , sizeof(cl_mem),&minMem);
	err |= clSetKernelArg(sceneBBoxKernel,4 , sizeof(cl_mem) , &maxMem);
	err |= clSetKernelArg(sceneBBoxKernel,5 , sizeof(cl_mem) , &meshMem);

}


//Grid Helper functions 
//this can't be broken
void RenderWindow::calculateVoxelSize()
{
	delta = box.max - box.min;
	deltaInv = 1.0f / delta;
	float voxelPerUnit = findVoxelsPerUnit();

	for (int i = 0; i < 3; i++)
	{
		numberOfVoxels[i] = (int)(delta[i] * voxelPerUnit);
		numberOfVoxels[i] = glm::clamp((int) numberOfVoxels[i], MIN_VOXELS,MAX_VOXELS);
		voxelWidth[i] = delta[i]/ numberOfVoxels[i];
		voxelInvWidth[i] = (voxelWidth[i] == 0.0f) ? 0.0f : 1.0f / voxelWidth[i];
	}
	//64 57 48 
	totalVoxels = numberOfVoxels[0] * numberOfVoxels[1]* numberOfVoxels[2];

	cout << numberOfVoxels[0] << " , " << numberOfVoxels[1] << ", " <<numberOfVoxels[2] << endl;



}
float RenderWindow::findVoxelsPerUnit()
{
	int maxAxis = box.maxExtent();
	float invWidth = 1.0f/delta[maxAxis];
	float cubeRoot = 3.0f * pow((float) objects.size(),invWidth);
	return cubeRoot * invWidth;

}
