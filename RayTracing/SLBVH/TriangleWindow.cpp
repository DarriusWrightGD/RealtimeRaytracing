#include "TriangleWindow.h"


TriangleWindow::TriangleWindow(void) : program(0), frame(0)
{
}


TriangleWindow::~TriangleWindow(void)
{
}


GLuint TriangleWindow::loadShader(GLenum type, const char * source)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &source , 0 );
	glCompileShader(shader);
	return shader;
}

void TriangleWindow::initialize()
{
	program  = new QOpenGLShaderProgram(this);
	program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);

	program->link();
    m_posAttr = program->attributeLocation("posAttr");
    m_colAttr = program->attributeLocation("colAttr");
    m_matrixUniform = program->uniformLocation("matrix");
}

void TriangleWindow::render()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT);

    program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);
    matrix.rotate(100.0f * frame / screen()->refreshRate(), 0, 1, 0);

    program->setUniformValue(m_matrixUniform, matrix);

    GLfloat vertices[] = {
        0.0f, 0.707f,
        -0.5f, -0.5f,
        0.5f, -0.5f
    };

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    glVertexAttribPointer(m_posAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(m_colAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    program->release();

    ++frame;
}