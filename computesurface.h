#ifndef COMPUTESURFACE_H
#define COMPUTESURFACE_H

#include <QOffscreenSurface>
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Core>

QT_BEGIN_NAMESPACE
class QOpenGLContext;
QT_END_NAMESPACE


class ComputeSurface;

//---------------------------------------------------------------------
//ComputeCommon
//Some common functions for support surface
//Used in ComputeBuffer and ComputeShader
//---------------------------------------------------------------------
class ComputeCommon {
public:

protected:
    void setup_surface(ComputeSurface *surface);
    ComputeSurface *surface_ = nullptr;

    void gl_assert(QString message); //Check openGL error
    void xassert(bool condition, QString message); //Check Qt wrapper error
    void activate_context();

    QOpenGLFunctions_4_3_Core *gl();
};

//Class for warping GPU buffer, will it with values from CPU and load to CPU after computations
class ComputeBuffer: public ComputeCommon {
public:
    void setup(ComputeSurface *surface);
    void allocate(void *data, int size_bytes);
    void read_to_cpu(void *data, int size_bytes);
    void clear();

    //Bind buffer to shader by specifying its binding index:
    //Shader:
    //    layout(std430, binding = 0) buffer Buf
    //    { float buf[]; };
    void bind_for_shader(int binding_index);

    //Unbind - not tested
    void unbind();

protected:
    QOpenGLBuffer shader_buffer_;
};

//---------------------------------------------------------------------
//ComputeShader
//Class for maintaining compute shaders
//---------------------------------------------------------------------
class ComputeShader: public ComputeCommon {
public:
    void setup(QString shader_file, ComputeSurface *surface);

    //Call this to set up uniforms
    void begin();

    //Access this to set up uniforms
    QOpenGLShaderProgram &program();

    //Call this to perform computing
    void compute(int NX, int NY = 1, int NZ = 1);   //NX,NY,NZ - number of groups in X,Y,Z dimensions

    //Call after computing end
    void end();

protected:
    QOpenGLShaderProgram program_;
};

//---------------------------------------------------------------------
//ComputeSurface
//Surface for maintaining OpenGL context
//Compute shaders and buffers will use it for enabling OpenGL context at operations
//It's subclass of QOffscreenSurface, it's required to have such thing by Qt work with OpenGL
//(Also can use QWindow, but in our case it's not needed, we want non-graphical computations)
//Note: QOffscreenSurface can work in non-main thread,
//but its "create" must be called from main thread
//---------------------------------------------------------------------
class ComputeSurface: public QOffscreenSurface, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit ComputeSurface();
    ~ComputeSurface();

    //Initialize OpenGL context and load shader, must be called before computing
    void setup();

    //Switch to OpenGL context - required before most operations
    void activate_context();

    void gl_assert(QString message); //Check openGL error
    void xassert(bool condition, QString message); //Check Qt wrapper error

    QOpenGLFunctions_4_3_Core *gl() { return gl43; }
private:
    //OpenGL context
    QOpenGLContext *m_context = nullptr;        //will be deleted automatically
    //OpenGL extra functions
    QOpenGLFunctions_4_3_Core *gl43 = nullptr;  //should't delete this!
};

#endif // COMPUTESURFACE_H
