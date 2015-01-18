#pragma once


#include <QtOpenGL\qglfunctions.h>
#include <QtOpenGL\qgl.h>
#include <QtOpenGLExtensions\qopenglextensions.h>
#include <QtGui\qwindow.h>
#include <QtGui\qevent.h>
#include <QtGui\qopenglpaintdevice.h>
#include <QtCore\qcoreapplication.h>

class OpenGLWindow : public QWindow,protected QOpenGLFunctions
{
	Q_OBJECT

public:
	explicit OpenGLWindow(QWindow * parent = 0);
	~OpenGLWindow(void);

	virtual void render(QPainter * painter);
	virtual void render();
	virtual void initialize();

	void setAnimating(bool animating);

public slots:
	void renderLater();
	void renderNow();

protected:
	bool event(QEvent * e);
	void exposeEvent(QExposeEvent * e);


private:
	bool updatePending;
	bool animating;


	
	QOpenGLPaintDevice * device;

protected:
	QOpenGLContext * glContext;
};

