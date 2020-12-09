#include "openglwindow.h"

#include <QtCore/QCoreApplication>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#include <QOpenGLExtraFunctions>
#include <QDebug>
#include <QFile>


//---------------------------------------------------------------------
OpenGLWindow::OpenGLWindow(QWindow *parent)
    : QWindow(parent)
    , m_animating(false)
    , m_context(0)
    , m_device(0)
{
    setSurfaceType(QWindow::OpenGLSurface);
}

//---------------------------------------------------------------------
OpenGLWindow::~OpenGLWindow()
{
    delete m_device;
}

//---------------------------------------------------------------------
void OpenGLWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}


//---------------------------------------------------------------------
void OpenGLWindow::initialize_context() {
    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());  //it's set in main
        /*format.setSamples(16);
        format.setMajorVersion(4);
        format.setMinorVersion(3);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setOption(QSurfaceFormat::DebugContext);
        */

        m_context->create();

        //switch to using context
        m_context->makeCurrent(this);

        initializeOpenGLFunctions();

        extra = new QOpenGLExtraFunctions(m_context);
        extra->initializeOpenGLFunctions();

        gl43 = m_context->versionFunctions<QOpenGLFunctions_4_3_Core>();
        gl_assert("Error creating gl43");


        initialize_app();
    }
}

//---------------------------------------------------------------------
void OpenGLWindow::renderNow()
{
    //qDebug() << "renderNow";
    if (!isExposed())
        return;

    initialize_context();

    //switch to using context
    m_context->makeCurrent(this);

    //custom rendering
    render_app();

    //computing!
    compute();

    //output to screen
    m_context->swapBuffers(this);

    //add request for immediate re-render to queue
    //if (m_animating)
    //    renderLater();
}
//---------------------------------------------------------------------

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

void OpenGLWindow::initialize_app()
{
    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    m_program->link();
    m_posAttr = m_program->attributeLocation("posAttr");
    m_colAttr = m_program->attributeLocation("colAttr");
    m_matrixUniform = m_program->uniformLocation("matrix");


}

//---------------------------------------------------------------------
void OpenGLWindow::render_app()
{
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT);

    m_program->bind();

    QMatrix4x4 matrix;
    matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);
    matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

    m_program->setUniformValue(m_matrixUniform, matrix);

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

    m_program->release();

    ++m_frame;
}

//---------------------------------------------------------------------
void OpenGLWindow::renderLater()
{
    //qDebug() << "renderLater";
    requestUpdate();
}

//---------------------------------------------------------------------
//check opengl errors
void OpenGLWindow::gl_assert(QString message) {
    GLenum error = GL_NO_ERROR;
    do {
        error = extra->glGetError();
        if (error != GL_NO_ERROR) {
            qDebug() << message << ", OpenGL error code " << error;
        }
    } while (error != GL_NO_ERROR);

}


//---------------------------------------------------------------------
bool OpenGLWindow::event(QEvent *event)
{

    switch (event->type()) {
    case QEvent::UpdateRequest:
        //qDebug() << "event";
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

//---------------------------------------------------------------------
void OpenGLWindow::exposeEvent(QExposeEvent *event)
{
    //qDebug() << "exposeEvent";
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

//---------------------------------------------------------------------
void OpenGLWindow::xassert(bool condition, QString message) {
    if (!condition) {
        qDebug() << message;
    }
}

//---------------------------------------------------------------------
//Based on https://forum.qt.io/topic/104448/about-buffer-for-compute-shader/6
void OpenGLWindow::compute() {

    // Compute shader init
    QString shader_file = ":/shader/compute_shader.csh";
    xassert(program.addShaderFromSourceFile(QOpenGLShader::Compute, shader_file),
            "Can't load shader file " + shader_file);

    //compute_shader->addShaderFromSourceCode(QOpenGLShader::Compute, text);
    xassert(program.link(), "Can't link compute shader");

    // Test buffers
    const int N = 23;
    float bbb[N];

    // SSBO init buffer
    m_context->makeCurrent(this);
    xassert(SSBO.create(), "Error at SSBO.create()");
    xassert(SSBO.bind(), "Error at SSBO.bind()");
    SSBO.allocate(bbb, sizeof(bbb));
    SSBO.setUsagePattern(QOpenGLBuffer::DynamicCopy);

    //Gives error, but is required
    gl43->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO.bufferId());
    gl_assert("Error at glBindBufferBase");

    // Multi use compute shader
    //for (int i=1; i<10; i++){
    // The data in the SSBO buffer should be placed strictly before the calculations
    // and updated every time as the calculations are started.

    xassert(program.bind(), "Can't bind compute shader");

    //SSBO.bind();

    gl43->glDispatchCompute(N, 1, 1);		//Run computation
    gl_assert("glDispatchCompute error");
    gl43->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);	//Wait finish computation
    gl_assert("glMemoryBarrier error");

    //SSBO.release();

    program.release();
    //}

    //Read buffer
    memcpy(bbb, SSBO.map(QOpenGLBuffer::ReadWrite), sizeof(bbb));
    qDebug() << "Output buffer: ";
    for (int i=0; i<N; i++) {
        qDebug() << "  " << bbb[i];
    }

}

//---------------------------------------------------------------------
struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};


void OpenGLWindow::computing() {
/*
    int n = 4;  //number of vertices
    VertexData data[] = {
        // Vertex data for face 0
        {QVector3D(-1.0f, -1.0f,  1.0f), QVector2D(0.0f, 0.0f)},  // v0
        {QVector3D( 1.0f, -1.0f,  1.0f), QVector2D(0.33f, 0.0f)}, // v1
        {QVector3D(-1.0f,  1.0f,  1.0f), QVector2D(0.0f, 0.5f)},  // v2
        {QVector3D( 1.0f,  1.0f,  1.0f), QVector2D(0.33f, 0.5f)}, // v3
    };
    int bytes = n * sizeof(VertexData);

    //ofBufferObject in openFrameworks

    //create buffer
    GLuint id;
    glGenBuffers(1,&id);
    gl_assert("glGenBuffers");

    GLenum target = GL_ARRAY_BUFFER;
    GLenum usage = GL_DYNAMIC_DRAW;

    //bind and fill buffer
    glBindBuffer(target, id);
    gl_assert("glBindBuffer");
    glBufferData(target, bytes, data, usage);
    gl_assert("glBufferData");

    //unbind buffer
    glBindBuffer(target, 0);
    gl_assert("unbind buffer");

    //bind to shader
    GLuint index = 0;
    GLenum buffer_base_target = GL_SHADER_STORAGE_BUFFER;
    extra->glBindBufferBase(buffer_base_target,index,id);
    gl_assert("glBindBufferBase");

    //start computing
    compute_shader->bind();
    gl_assert("compute shader bind");

    //compute_shader->setUniformValue(m_matrixUniform, matrix);
    //compute_shader->
    int x = n;
    int y = 1;
    int z = 1;
    extra->glDispatchCompute(x,y,z);
    gl_assert("glDispatchCompute");

    extra->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    gl_assert("glMemoryBarrier");

    compute_shader->release();

    //read buffer to CPU
    glBindBuffer(target, id);
    GLenum access = GL_READ_WRITE;
    void *ret = extra->glMapBuffer(target,access);
*/
}


//---------------------------------------------------------------------
void OpenGLWindow::setAnimating(bool animating)
{
    //qDebug() << "setAnimating";
    m_animating = animating;

    if (animating)
        renderLater();
}

//---------------------------------------------------------------------

