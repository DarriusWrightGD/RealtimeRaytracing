//

#define _CRT_SECURE_NO_WARNINGS
//list all opencl programs here
#define PROGRAM_FILE "color_image.opencl"
#define KERNEL_FUNC "color_image"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <QtWidgets\qwidget.h>
#include <QtWidgets\qapplication.h>
#include <QtWidgets\qlabel.h>
#include <QtCore\qelapsedtimer.h>
#include "RenderWindow.h"

#ifdef MAC
#include <OpenCL/cl.h>
#else  
#include <CL/cl.h>
#endif


#include <cstdlib>
#include <iostream>
#include <ctime>
#include "MainWindow.h"
#include "CudaWidget.h"
using std::cout;
using std::endl;

int main(int argc, char * argv []) 
{
	//srand(65776);
	srand(2433244);
	QApplication app(argc,argv);
	
	RenderWindow * renderwindow = new RenderWindow();
	MainWindow window(renderwindow);
	window.show();


	/*QSurfaceFormat format;
	format.setSamples(16);


	TriangleWindow window;
	window.setFormat(format);
	window.resize(640,480);
	window.show();
	window.setAnimating(true);*/


	return app.exec();
}

