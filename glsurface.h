#ifndef GLSURFACE_H
#define GLSURFACE_H

//general
#include <QtGui/QWindow>
#include <QOffscreenSurface>
#include <QtGui/QOpenGLFunctions>

//specifics
#include <QtGui/QGuiApplication>
#include <QtGui/QMatrix4x4>
#include <QtGui/QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QtGui/QScreen>

#include <QtCore/qmath.h>
#include <QOpenGLFunctions_4_3_Core>

QT_BEGIN_NAMESPACE
class QPainter;
class QOpenGLContext;
class QOpenGLPaintDevice;
QT_END_NAMESPACE


//Note: QOffscreenSurface can work in non-main thread,
//but its "create" must be called from main thread

class GLSurface : public QOffscreenSurface, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit GLSurface();
    ~GLSurface();

    void compute();

private:
    QOpenGLContext *m_context = nullptr;
    QOpenGLPaintDevice *m_device = nullptr;

    void initialize_context();

    QOpenGLShaderProgram program;
    QOpenGLBuffer  SSBO;

    //QOpenGLExtraFunctions
    QOpenGLFunctions_4_3_Core *gl43 = nullptr;

    void gl_assert(QString message);
    void xassert(bool condition, QString message);
};

#endif // GLSURFACE_H
