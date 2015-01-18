#pragma once

#include <QtWidgets\qwidget.h>
#include <QtWidgets\qlayout.h>
#include <QtWidgets\qlabel.h>
#include <QtWidgets\qgraphicsview.h>
#include <QtWidgets\qgraphicsscene.h>
#include <QtOpenGL\qgl.h>
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

#include <QtOpenGL\qglfunctions.h>
#include <QtGui\qwindow.h>
#include "OpenGLWindow.h"
#include <QtGui\qopenglframebufferobject.h>
#include <CL\cl_gl.h>
#include <Timer.h>

#include "Morton.h"


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
	void addObject(Object sphere);


	//	void initialize()override;
	//	void render()override;

private:
	//	QOpenGLFramebufferObject * frameBuffer;
	//	GLuint frameBufferId;
	//	void createFrameBuffer();

	void initializeMemory();
	Camera camera;

	cl_int lengthOfObjects; 
	void updateDrawScene();
	void updateBBox();
	void updateCells();
	BBox box;
	static const cl_float MIN;
	static const cl_float MAX;
	static const cl_uint NUMBER_OF_SPHERES;

	static const cl_uint bitsbyte = 8u;
	static const  cl_uint R = (1 << bitsbyte);
	static const  cl_uint R_MASK = 0xFFu;


	cl_int nx,ny,nz , numCells, nextPowerOfCells, numberOfCellObjects;
	cl_float wx,wy,wz, s;
	const cl_float multi;
	QImage readImage;

	QElapsedTimer profileTimer;

	//vectors
	vector<cl_int> cellIndices;
	vector<cl_int> objectIndices;
	vector<cl_int> cellIncrements;
	vector<BBox> cBoxes;
	vector<cl_uint> cells;
	vector <MortonNode> mortonNodes;

	cl_image_format clImageFormat;

	void releaseUpdate();

	void construct();
	void initializeCL();
	void initializeObjects();
	void initializeProgram();
	void initializeSceneBBox();
	void initializeCells();
	void initializeCellObjects();
	void initializeDrawScene();

	cl_program buildProgram(std::string fileName [], int numberOfFiles);
	void handleKeyInput();

	INT64 time;
	QTimer updateTimer;
	QElapsedTimer timer;

	QHBoxLayout * layout;
	QLabel imageLabel;
	vector<Object> objects;
	vector<Light> lights;


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
	cl_kernel initializeMortonCodesKernel;
	cl_kernel sortMortonKernel;
	cl_kernel drawSceneKernel;
	cl_kernel findObjectCellsKernel;


	//radix sort kernels
	cl_kernel histogramKernel;
	cl_kernel permuteKernel  ;
	cl_kernel unifiedBlockScanKernel ;
	cl_kernel blockScanKernel ;
	cl_kernel prefixSumKernel;
	cl_kernel blockAddKernel ;
	cl_kernel mergePrefixSumsKernel;

	//radix sort mem
	cl_mem unsortedDataMem    ;
	cl_mem histogramMem       ;
	cl_mem scannedHistogramMem;
	cl_mem sortedDataMem      ;
	cl_mem sumInMem          ;
	cl_mem sumOutMem         ;
	cl_mem summaryInMem      ;
	cl_mem summaryOutMem     ;

	//members...
	cl_uint radixGroupSize;// = 64;     
	cl_uint radixBinSize;//  = 256;
	cl_uint radixDataSize;// =  (1<<14);

	//radix methods
	void initializeMortonMem();
	void releaseRadixMem();
	void computeRadixGroups();
	void computeHistogram(int currentByte);
	void computeRankingNPermutations(int currentByte, size_t groupSize);
	void computeBlockScans();
	void radixSort();

	//update release mem
	cl_mem objectIndicesMem;
	cl_mem cellIndicesMem;
	cl_mem cellIncrementsMem;
	cl_mem clImage;
	cl_mem cellsMem;
	cl_mem cellsBoxMem;
	cl_mem minMem;
	cl_mem maxMem;
	cl_mem mortonMem;

	cl_mem writeCLImage;
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
	size_t initCellWorkSize[2];

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

	private slots:
		void updateScene();


};

