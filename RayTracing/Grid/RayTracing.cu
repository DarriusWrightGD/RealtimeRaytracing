#include <iostream> 
#include <vector> 
#include <cuda_runtime.h> 
#include <cuda.h>
#include <thrust\detail\mpl\math.h>
#include <math_functions.h>
#include <common_functions.h>
#include "helper_math.h"
#include <QtWidgets\qwidget.h>
#include <QtWidgets\qapplication.h>
#include <QtCore\qtimer.h>
#include "RenderWindow.h"
#include "MainWindow.h"
#include "CudaWidget.h"

#pragma comment(lib, "cudart") 


using std::cerr; 
using std::cout; 
using std::endl; 
using std::exception; 
using std::vector; 


static void HandleError( cudaError_t err,
						const char *file,
						int line ) {
							if (err != cudaSuccess) {
								printf( "%s in %s at line %d\n", cudaGetErrorString( err ),
									file, line );
								system("pause");
								exit( EXIT_FAILURE );
							}
}
#define HANDLE_ERROR( err ) (HandleError( err, __FILE__, __LINE__ ))
#define INF 10000000

struct CSphere
{
	float3 rgb;
	float3 position;
	float radius;

	__device__ float hit(float2 pixel, float * n)
	{
		float2 delta = pixel - make_float2(position.x,position.y);
		if(dot(delta, delta) < radius * radius)
		{
			float dz = sqrtf(radius * radius - (delta.x * delta.x - delta.y * delta.y));
			return dz + position.z;
		}

		return -INF;
	}
};


__global__ void colorKernel(uchar4 * image)
{
	int x = blockIdx.x;
	int y = blockIdx.y;

	int offset = x + y * gridDim.x;
	//image[offset] =make_uchar4( 100,0,0,255);
	image[offset].x = 100;//make_uchar4( 100,0,0,255);
	image[offset].w = 255;//make_uchar4( 100,0,0,255);

}


void intitializeDev(uchar4 ** dev , int size)
{	
	HANDLE_ERROR(cudaMalloc(dev, size));
}


void copyImage(uchar4 * dev, unsigned char * dst, int size)
{
	//	HANDLE_ERROR(cudaMemcpy(dst, dev,size, cudaMemcpyDeviceToHost));
	HANDLE_ERROR(cudaMemcpy(dst, dev,size, cudaMemcpyDeviceToHost));
}


void draw(int width, int height , uchar4 * dev)
{
	dim3 grid(width, height,1);
	colorKernel<<< grid , 1 >>>(dev);
}

void release(uchar4 * dev)
{
	cudaFree(dev);
}


void update()
{

}



//class CudaWidget : public QWidget
//{
//public:
//	CudaWidget(uchar4 ** dev  , uchar * d) 
//	{
//		setMinimumSize(640,480);
//		QLayout * main = new QHBoxLayout();
//		setLayout(main);
//		windowWidth = width();
//		windowHeight = height();
//		size = windowWidth * windowHeight * 4;
//		readBuffer = d;
//		QImage r(readBuffer,windowWidth,windowHeight,QImage::Format::Format_RGBA8888);
//		readImage = r;
//
//		devReadBuffer = dev;
//		connect(&updateTimer,&QTimer::timeout,this, &CudaWidget::update);
//		updateTimer.start();
//
//		main->addWidget(&imageLabel);
//	}
//
//
//	~CudaWidget(void)
//	{
//		//delete [] readBuffer;
//		//release(*devReadBuffer);
//	}
//
//
//	int windowWidth;
//	int windowHeight;
//	float3 color;
//	QImage readImage;
//	uchar * readBuffer;
//	uchar4 ** devReadBuffer;
//	QLabel imageLabel;
//	QTimer updateTimer;
//	int size;
//
//	void update(void)
//	{
//
//		draw(640,480,*devReadBuffer);
//		copyImage(*devReadBuffer, readBuffer,size);
//
//		imageLabel.setPixmap(QPixmap::fromImage(QImage(readBuffer,640,480,QImage::Format::Format_RGBA8888)));
//	}
//
//
//};
#include <cuda_texture_types.h>
#include <texture_types.h>

//int main(int argc, char * argv []) 
//{
//
//	QApplication app(argc,argv);
//	int size  = 640 * 480 * 4;
//	unsigned char * d = new unsigned char[(size)];
//	uchar4 * dev = nullptr;
//	HANDLE_ERROR(cudaMalloc(&dev, size));
//	//intitializeDev(&dev,size);
//	draw(640,480,dev);
//	//copyImage(dev, d,size);
//	text
//	HANDLE_ERROR(cudaMemcpy(d, dev,size, cudaMemcpyDeviceToHost));
//
//	//QLabel c;
//	//c.setPixmap(QPixmap::fromImage(QImage(d,640,480,QImage::Format::Format_RGBA8888)));
//
//	CudaWidget c(&dev, d);
//	c.show();
//	//QLabel cl;
//	//cl.setPixmap(QPixmap::fromImage(QImage(d,640,480,QImage::Format::Format_RGBA8888)));
//	//cl.show();
//	return app.exec();
//}