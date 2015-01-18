#pragma once

#include "OpenGLWindow.h"
#include <QtGui\qopenglshaderprogram.h>
#include <QtGui\qscreen.h>


static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = matrix * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";

class TriangleWindow : public OpenGLWindow
{
public:
	TriangleWindow(void);
	~TriangleWindow(void);


	void initialize()override;
	void render()override;


private:
	GLuint loadShader(GLenum type , const char *source );
	GLuint m_posAttr;
	GLuint m_colAttr;
	GLuint m_matrixUniform;
	QOpenGLShaderProgram * program;
	int frame;
};

