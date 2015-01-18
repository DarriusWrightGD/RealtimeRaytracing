#pragma once

#include <QtWidgets\qwidget.h>
#include <QtWidgets\qlayout.h>
#include <QtWidgets\qlabel.h>
#include <QtWidgets\qgraphicsview.h>
#include <QtWidgets\qgraphicsscene.h>

#include <QtCore\qtimer.h>
#include <Windows.h>
#include <iostream>
#include <Lights\Light.h>
#include <Objects\Object.h>
#include <Objects\Sphere.h>
#include <vector>
#include <Random.h>
#include<QtCore\qelapsedtimer.h>
#include <Camera\Camera.h>
#include <string>
#include <QtGui\qwindow.h>

#include <QtCore\qdebug.h>

#include <Objects\Triangle.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <Helper.h>
#include <Exception\OpenCLExceptions.h>
#include <ctime>
#include <stdio.h>
#include <QtCore\qdatetime.h>
#include <Objects\Mesh.h>
#include "GameObject.h"
#include <QtQml\qqmllist.h>
#include "GameObjectContainer.h"
#include "Octree.h"
#include "KDTree.h"
#include <QtGui\qevent.h>

using std::vector;
using std::cout;
using std::endl;
class RenderWindow :public QWidget// public OpenGLWindow
{
	Q_OBJECT;
	
public:
	RenderWindow(void);
	~RenderWindow(void);

	
	void resizeEvent(QResizeEvent * e)override;
	void addLight(Light light);
	void addObject(std::string path);
	void addSphere();
	float getFPS();
	float getInterval();
	Random random;
	void setSamples(int samples = 4);
	void setMaxBounce(int bounces);

	void setShadowsEnabled(bool enabled);
	void setReflectionsEnabled(bool enabled);
	void setRefractionsEnabled(bool enabled);
	bool isShadowsEnabled()const;
	bool isReflectionsEnabled()const;
	bool isRefractionsEnabled()const;
	vector<Light> lights;
	void changeLightType(QString index);
	QList<GameObject * > gameObjectList;
	GameObject * lightObject;
	GameObject * cameraObject;
	GameObjectContainer gameObjectContainer;

//	void initialize()override;
//	void render()override;


	std::vector<Mesh> meshes;
	int samples;
	int numberOfReflections;
	int numberOfRefractions;
	int maxDepth;




	bool globalIlluminationEnabled();
	void setGlobalIllumination(bool enabled);
	int sampleSquared;
	Camera camera;


	public  slots:
	void changeLightType(int index);
	void changeCameraType(int index);

	signals:
	void updateList();
	void keyReleaseEvent(QKeyEvent * e) override;
	void keyPressEvent(QKeyEvent * e) override;
private:
	void scene1();
	void scene2();
	void scene3();
	void scene4();
	void scene5();
	void initializeGameObjectList();
	void clearScene();
	void sceneInitialize();
	void sceneRelease();
	cl_mem stackMem;
	OctreeManager octManager;
	KDTreeManager kdManager;
	int numberOfMeshes;
	cl_mem kdTreeMem;
	cl_mem kdTreeIndicesMem;
	glm::vec4 backgroundColor;
	float currentTime;
	int randomInt;
	long randSeed;
	bool initialized;



	//Grid functions
	void calculateVoxelSize();
	float  findVoxelsPerUnit();

	//Grid data
	glm::vec3 numberOfVoxels;
	glm::vec3 voxelWidth;
	glm::vec3 voxelInvWidth;
	glm::vec3 delta;
	glm::vec3 deltaInv;
	int totalVoxels;


	//Grid Const
	static const int MAX_VOXELS = 64;
	static const int MIN_VOXELS = 1;


	float fps;
	float interval;

	void addMesh(std::string filePath, glm::vec3 position = glm::vec3(0.0f,0.0f,10.0f) , MaterialType type = DIFFUSE);
	

//	QOpenGLFramebufferObject * frameBuffer;
//	GLuint frameBufferId;
//	void createFrameBuffer();
	std::vector<Triangle> triangles;

	
	void updateDrawScene();
	void updateBBox();
	void updateCells();
	BBox box;
	static const cl_float MIN;
	static const cl_float MAX;
	static const cl_uint NUMBER_OF_SPHERES;
	static const cl_uint flags = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType;
		
	cl_int  numCells, nextPowerOfCells, numberOfCellObjects;
	const cl_float multi;
	QImage readImage;

	QElapsedTimer profileTimer;
	
	//vectors
	vector<cl_int> cellIndices;
	vector<cl_int> objectIndices;
	vector<cl_int> cellIncrements;
	vector<cl_float3> minArr;
	vector<cl_float3> maxArr;
	vector<std::string> objectPaths;
	vector<cl_uint> cells;



	void releaseUpdate();

	void construct();
	void initializeCL();
	void initializeProgram();
	void initializeMemory();
	void initializeSceneBBox();
	void initializeCells();
	void initializeCellObjects();
	void initializeDrawScene();

	void setUpDrawSceneArgs();
	void setUpShadowArgs();
	void setUpReflectionArgs();
	void setUpRefractionArgs();
	void setUpSceneBoxArgs();
	void setUpCellArgs();
	void setUpCellObjectArgs();

	void releaseDrawScene();
	void releaseCells();
	void releaseBBox();



	cl_program buildProgram(std::string fileName [], int numberOfFiles);
	void handleKeyInput();

	INT64 time;
	QTimer updateTimer;
	QElapsedTimer timer;

	QHBoxLayout * layout;
	QLabel imageLabel;
	vector<Object> objects;


	//cl_event 

	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_int i, err;
	cl_program program;
	FILE *program_handle;
	char *program_log;
	size_t  log_size;
	


	cl_kernel sceneBBoxKernel;
	cl_kernel initializeCellsKernel;
	cl_kernel drawSceneKernel;
	cl_kernel drawShadowRaysKernel;
	cl_kernel drawReflectionRaysKernel;
	cl_kernel drawRefractionRaysKernel;
	cl_kernel findObjectCellsKernel;
	cl_kernel emitPhotonKernel;
	cl_kernel clearPhotonKernel;


	void drawSecondaryRays();

	cl_uint shadowsEnabled;
	cl_uint reflectionsEnabled;
	cl_uint refractionsEnabled;
	cl_uint globalIllumination;

	//update release mem
	cl_mem objectIndicesMem;
	cl_mem octreeMem;
	cl_mem objectPhotonCount;
	cl_mem cellIndicesMem;
	cl_mem cellIncrementsMem;
	cl_mem clImage;
	cl_mem cellsMem;
	cl_mem minMem;
	cl_mem maxMem;
	cl_mem trianglesMem;
	cl_mem meshMem;
	cl_mem writeCLImage;
	cl_mem depthBuffer;
	cl_mem lightMem;
	cl_mem sumMem;
	cl_mem boundingBoxMem;
	cl_mem objectMem;

	cl_sampler sampler;
	cl_int windowWidth;
	cl_int windowHeight;
	cl_int numberOfLights;
	cl_int numberOfObjects;
	size_t globalWorkSize[2];
	size_t initCellWorkSize;
	size_t dCellWork[2];
	
	size_t sceneBBoxGlobalWorkSize;

	size_t origin[3];
	size_t region[3];// = {width(),height(),1};
	uchar * readBuffer;



	int inline roundUp(int groupSize, int globalSize)
	{
		int r = globalSize % groupSize;


		return (r == 0) ?  globalSize: globalSize + groupSize -r;
	}

	int inline nextPowerOfTwo(int number)
	{
		return pow(2, ceil(log(number)/log(2)));
	}

	inline int findVoxelIndex(glm::vec3 position , glm::vec3 cellDimensions)//
	{
		return position.x + position.y * (int)cellDimensions.x + position.z * (int)cellDimensions.x * (int)cellDimensions.y;
	}

	inline glm::vec3 voxelToPosition(BBox sceneBox, glm::vec3 position, glm::vec3 width)//
	{
		return sceneBox.min + (position * width);
	}

	inline glm::vec3 positionToVoxel(glm::vec3 position,  glm::vec3 invWidth, glm::vec3 numberOfVoxels , BBox sceneBox)//
	{
		glm::vec3 dif = (position - sceneBox.min) + 0.0001f;
		glm::vec3 voxelPosition = (dif * invWidth);
		glm::vec3 nVoxels = (numberOfVoxels);
		voxelPosition = glm::clamp(voxelPosition,glm::vec3(0.0f,0.0f,0.0f) , numberOfVoxels);
		voxelPosition = glm::floor(voxelPosition);
		return voxelPosition;
	}
	inline glm::vec3 findVoxelPosition(int index , glm::vec3 cellDimensions)//
	{
		glm::vec3 position;
		position.z = index / ((int)(cellDimensions.x* cellDimensions.y));
		position.y = (index % ((int)(cellDimensions.x* cellDimensions.y))) / ((int)cellDimensions.x);
		position.x = index - position.y * cellDimensions.x - position.z *cellDimensions.x *cellDimensions.y;
		return glm::floor(position);
	}

public slots:
	void updateScene();
private slots:
	//void openFile();
	
};

