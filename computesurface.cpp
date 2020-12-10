#include "computesurface.h"

#include <QtCore/QCoreApplication>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#include <QOpenGLExtraFunctions>
#include <QDebug>
#include <QFile>


//---------------------------------------------------------------------
//ComputeCommon
//---------------------------------------------------------------------
void ComputeCommon::setup_surface(ComputeSurface *surface) {
    surface_ = surface;
    activate_context();
}

//---------------------------------------------------------------------
void ComputeCommon::gl_assert(QString message) { //Check openGL error
    if (surface_) surface_->gl_assert(message);
}

//---------------------------------------------------------------------
void ComputeCommon::xassert(bool condition, QString message) { //Check Qt wrapper error
    if (surface_) surface_->xassert(condition, message);
}

//---------------------------------------------------------------------
void ComputeCommon::activate_context() {
    if (surface_) surface_->activate_context();
}

//---------------------------------------------------------------------
QOpenGLFunctions_4_3_Core *ComputeCommon::gl() {
    return surface_->gl();
}

//---------------------------------------------------------------------
//ComputeBuffer
//---------------------------------------------------------------------
void ComputeBuffer::setup(ComputeSurface *surface) {
    setup_surface(surface);

    activate_context();
    xassert(shader_buffer_.create(), "Error at shader_buffer_.create()");
    xassert(shader_buffer_.bind(), "Error at shader_buffer_.bind()");
    shader_buffer_.setUsagePattern(QOpenGLBuffer::DynamicCopy);
}

//---------------------------------------------------------------------
void ComputeBuffer::allocate(void *data, int size_bytes) {
    activate_context();
    shader_buffer_.allocate(data, size_bytes);
}

//---------------------------------------------------------------------
void ComputeBuffer::clear() {
    activate_context();
    shader_buffer_.release();
}

//---------------------------------------------------------------------
void ComputeBuffer::read_to_cpu(void *data, int size_bytes) {
    activate_context();
    //Read buffer
    memcpy(data, shader_buffer_.map(QOpenGLBuffer::ReadWrite), size_bytes);

}

//---------------------------------------------------------------------
//Bind buffer to shader by specifying its binding index:
//Shader:
//    layout(std430, binding = 0) buffer Buf
//    { float buf[]; };
void ComputeBuffer::bind_for_shader(int binding_index) {
    surface_->gl()->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_index, shader_buffer_.bufferId());
    gl_assert("Error at glBindBufferBase");
}

//---------------------------------------------------------------------
//ComputeShader
//---------------------------------------------------------------------
//Initialize OpenGL context and load shader, must be called before computing
void ComputeShader::setup(QString shader_file, ComputeSurface *surface) {
    setup_surface(surface);

    //Not sure if it's required for loading shader...
    activate_context();
    //Load compute shader
    xassert(program_.addShaderFromSourceFile(QOpenGLShader::Compute, shader_file),
            "Can't load shader file " + shader_file);

    //compute_shader->addShaderFromSourceCode(QOpenGLShader::Compute, text);
    xassert(program_.link(), "Can't link compute shader");
}

//---------------------------------------------------------------------
//Call this to set up uniforms
void ComputeShader::begin() {
    activate_context();
    xassert(program_.bind(), "Can't bind compute shader");
}

//---------------------------------------------------------------------
//Access this to set up uniforms
QOpenGLShaderProgram &ComputeShader::program() {
    return program_;
}

//---------------------------------------------------------------------
//Call this to perform computing
//Based on https://forum.qt.io/topic/104448/about-buffer-for-compute-shader/6
void ComputeShader::compute(int NX, int NY, int NZ) {
    gl()->glDispatchCompute(NX, NY, NZ);		//Run computation
    gl_assert("glDispatchCompute error");
    gl()->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);	//Wait finish computation
    gl_assert("glMemoryBarrier error");
}

//---------------------------------------------------------------------
//Call after computing end
void ComputeShader::end() {
    program_.release();
}

//---------------------------------------------------------------------
//ComputeSurface
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
void ComputeSurface::setup() {
    //Initialize OpenGL context
    if (!m_context) {
        QSurfaceFormat format;
        //Request OpenGL 4.3
        format.setMajorVersion(4);
        format.setMinorVersion(3);

        //format.setSamples(16); //antialiasing samples
        //format.setProfile(QSurfaceFormat::CoreProfile);
        //format.setOption(QSurfaceFormat::DebugContext);
        setFormat(format);

        //Create Qt surface
        create();

        //Create OpenGL context
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());  //it's our "format"
        m_context->create();

        //Switch to OpenGL context
        activate_context();

        //Initialize OpenGL functions
        initializeOpenGLFunctions();

        //Get pointer to extra functions of 4.3
        gl43 = m_context->versionFunctions<QOpenGLFunctions_4_3_Core>();
        gl_assert("Error creating gl43");
    }
}

//---------------------------------------------------------------------
//Switch to OpenGL context - required before most operations
void ComputeSurface::activate_context() {
    xassert(m_context, "OpenGL context is not inited");
    if (!m_context) return;
    m_context->makeCurrent(this);
}

//---------------------------------------------------------------------

