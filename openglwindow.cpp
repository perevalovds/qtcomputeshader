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

    //computing!
    compute();

    //output to screen
    m_context->swapBuffers(this);

    //add request for immediate re-render to queue
    //if (m_animating)
    //    renderLater();
}
//---------------------------------------------------------------------


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

