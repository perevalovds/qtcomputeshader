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

    virtual void render(QPainter *painter);
    virtual void render_app();

    virtual void initialize_app();

    void setAnimating(bool animating);

    void compute();


public slots:
    void renderLater();
    void renderNow();

protected:
    bool event(QEvent *event) override;

    void exposeEvent(QExposeEvent *event) override;

private:
    bool m_animating;

    QOpenGLContext *m_context = nullptr;
    QOpenGLPaintDevice *m_device = nullptr;

    void initialize_context();

    void compute_execute();


    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_matrixUniform;

    QOpenGLShaderProgram *m_program = nullptr;
    int m_frame = 0;


    QOpenGLExtraFunctions *extra = nullptr;

    QOpenGLShaderProgram program;
    QOpenGLBuffer  SSBO;

    //QOpenGLExtraFunctions
    QOpenGLFunctions_4_3_Core *gl43 = nullptr;



    QOpenGLBuffer arrayBuf;

    void computing();

    void gl_assert(QString message);
    void xassert(bool condition, QString message);
};

