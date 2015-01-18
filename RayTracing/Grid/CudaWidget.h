#pragma once
#include <QtWidgets\qwidget.h>
#include <QtWidgets\qlayout.h>
#include <QtWidgets\qlabel.h>
#include <QtCore\qtimer.h>
#include <QtGui\qwindow.h>
#include <QtCore\qdebug.h>
#include <Objects\Triangle.h>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <Helper.h>
#include <Exception\OpenCLExceptions.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <QtCore\qelapsedtimer.h>

extern void intitializeDev(uchar4 ** dev , int size);
extern void copyImage(uchar4 * dev, unsigned char * dst, int size);
extern void draw(int width, int height ,uchar4 * dev);
extern void release(uchar4 * dev);
class CudaWidget : public QWidget
{
public:
	CudaWidget(uchar4 ** dev , uchar * readC);
	~CudaWidget(void);



	int windowWidth;
	int windowHeight;
	float3 color;
	QImage readImage;
	void update();
	uchar * readBuffer;
	uchar4 ** devReadBuffer;
	QLabel imageLabel;
	QTimer updateTimer;
	int size;
	QElapsedTimer timer;

};


