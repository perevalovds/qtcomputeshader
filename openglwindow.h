//general
#include <QtGui/QWindow>
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

class OpenGLWindow : public QWindow, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWindow(QWindow *parent = 0);
    ~OpenGLWindow();

    void compute();


public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event) override;

    void exposeEvent(QExposeEvent *event) override;

private:
    QOpenGLContext *m_context = nullptr;
    QOpenGLPaintDevice *m_device = nullptr;

    void initialize_context();




    QOpenGLExtraFunctions *extra = nullptr;

    QOpenGLShaderProgram program;
    QOpenGLBuffer  SSBO;

    //QOpenGLExtraFunctions
    QOpenGLFunctions_4_3_Core *gl43 = nullptr;

    void gl_assert(QString message);
    void xassert(bool condition, QString message);
};

