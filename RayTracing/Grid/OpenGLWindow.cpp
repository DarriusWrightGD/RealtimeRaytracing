#include "OpenGLWindow.h"


OpenGLWindow::OpenGLWindow(QWindow * parent)
	: QWindow(parent),updatePending(false),
	animating(false), glContext(0), device(0)

{
	setSurfaceType(QWindow::OpenGLSurface);
}


OpenGLWindow::~OpenGLWindow(void)
{
}


void OpenGLWindow::render(QPainter * painter)
{
	Q_UNUSED(painter);
}

void OpenGLWindow::initialize()
{

}

void OpenGLWindow::render()
{
	if(!device)
		device = new QOpenGLPaintDevice();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	device->setSize(size());
	QPainter painter(device);
	render(&painter);
}

void OpenGLWindow::renderLater()
{
	if(!updatePending)
	{
		updatePending = true;
		QCoreApplication::postEvent(this,new QEvent(QEvent::UpdateRequest));
	}
}


bool OpenGLWindow::event(QEvent * e)
{
	switch(e->type())
	{
	case QEvent ::UpdateRequest:
		updatePending = false;
		renderNow();
		return true;
	default:
		return QWindow::event(e);
	}
}

void OpenGLWindow::exposeEvent(QExposeEvent * e)
{
	Q_UNUSED(e);

	if(isExposed())
		renderNow();
}

void OpenGLWindow::renderNow()
{
	if(!isExposed())
	{
		return;
	}

	bool needsInitialized = false;

	if(!glContext)
	{
		glContext = new QOpenGLContext(this);
		glContext->setFormat(requestedFormat());
		glContext->create();

		needsInitialized = true;
	}

	glContext->makeCurrent(this);


	if(needsInitialized)
	{
		initializeOpenGLFunctions();
		initialize();
	}

	render();

	glContext->swapBuffers(this);

	if(animating)
		renderLater();
}



void OpenGLWindow::setAnimating(bool animating)
{
	this->animating = animating;

	if(animating)
		renderLater();
}
