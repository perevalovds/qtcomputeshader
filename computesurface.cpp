#include "computesurface.h"

#include <QtCore/QCoreApplication>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#include <QOpenGLExtraFunctions>
#include <QDebug>
#include <QFile>


//---------------------------------------------------------------------
//Note: QOffscreenSurface can work in non-main thread,
//but its "create" must be called from main thread

ComputeSurface::ComputeSurface()
    : QOffscreenSurface()
    , m_context(0)
{

}

//---------------------------------------------------------------------
ComputeSurface::~ComputeSurface()
{

}

//---------------------------------------------------------------------
//Check openGL error
void ComputeSurface::gl_assert(QString message) {
    GLenum error = GL_NO_ERROR;
    do {
        error = gl43->glGetError();
        if (error != GL_NO_ERROR) {
            qDebug() << message << ", OpenGL error code " << error;
        }
    } while (error != GL_NO_ERROR);

}

//---------------------------------------------------------------------
//Check Qt wrapper error
void ComputeSurface::xassert(bool condition, QString message) {
    if (!condition) {
        qDebug() << message;
    }
}

//---------------------------------------------------------------------
//Initialize OpenGL context and load shader, must be called before computing
void ComputeSurface::setup(QString shader_file) {
    //Initialize OpenGL context
    initialize_context();

    //Not sure if it's required for loading shader...
    activate_context();

    //Load compute shader
    xassert(program.addShaderFromSourceFile(QOpenGLShader::Compute, shader_file),
            "Can't load shader file " + shader_file);

    //compute_shader->addShaderFromSourceCode(QOpenGLShader::Compute, text);
    xassert(program.link(), "Can't link compute shader");
}

//---------------------------------------------------------------------
void ComputeSurface::initialize_context() {
    if (!m_context) {
        QSurfaceFormat format;
        //Request OpenGL 4.3
        format.setMajorVersion(4);
        format.setMinorVersion(3);

        //format.setSamples(16); //antialiasing samples
        //format.setProfile(QSurfaceFormat::CoreProfile);
        //format.setOption(QSurfaceFormat::DebugContext);
        setFormat(format);

        //Create surface
        create();

        //Create OpenGL context
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());  //it's our "format"
        m_context->create();

        //switch to using context, because initializeOpenGLFunctions requires it
        activate_context();

        //Initialize OpenGL functions
        initializeOpenGLFunctions();

        //Get pointer to extra functions of 4.3
        gl43 = m_context->versionFunctions<QOpenGLFunctions_4_3_Core>();
        gl_assert("Error creating gl43");
    }
}

//---------------------------------------------------------------------
void ComputeSurface::activate_context() {    //switches to its OpenGL context - required for most operations
    xassert(m_context, "OpenGL context is not inited");
    if (!m_context) return;
    m_context->makeCurrent(this);
}

//---------------------------------------------------------------------
//Based on https://forum.qt.io/topic/104448/about-buffer-for-compute-shader/6
void ComputeSurface::compute() {
    xassert(m_context, "No initialization!");
    if (!m_context) return;


    // Test buffers
    const int N = 23;
    float bbb[N];
    for (int i=0; i<N; i++) {
        bbb[i] = i;
    }

    qDebug() << "Input buffer: ";
    for (int i=0; i<N; i++) {
        qDebug() << "  " << bbb[i];
    }

    // SSBO init buffer
    activate_context();

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

