#include "CudaWidget.h"


CudaWidget::CudaWidget(uchar4 ** dev , uchar * d) 
{
	setMinimumSize(640,480);
	QLayout * main = new QHBoxLayout();
	setLayout(main);
	windowWidth = width();
	windowHeight = height();
	size = windowWidth * windowHeight * 4;
	readBuffer = d;
	QImage r(readBuffer,windowWidth,windowHeight,QImage::Format::Format_RGBA8888);
	readImage = r;

	devReadBuffer = dev;
	connect(&updateTimer,&QTimer::timeout,this, &CudaWidget::update);
	updateTimer.start();

	main->addWidget(&imageLabel);
}


CudaWidget::~CudaWidget(void)
{
	//delete [] readBuffer;
	//release(*devReadBuffer);
}


void CudaWidget::update(void)
{
	timer.start();
	draw(640,480,*devReadBuffer);
	copyImage(*devReadBuffer, readBuffer,size);

	imageLabel.setPixmap(QPixmap::fromImage(readImage));
	float interval = (timer.elapsed()/1000.0f);
	float fps = 1.0f/interval;
	setWindowTitle(QString::number(fps));
}