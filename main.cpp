/*
 * Example of using compute shaders in Qt for fastening computations
 * widhout graphics output.
 * This is example is based on "openglwindow" Qt example
 * Denis Perevalov, github.com/perevalovds
 *
 * also see:
 * 1) Qt "cube" example
 * 2) Compute shaders and buffers in Qt: https://forum.qt.io/topic/104448/about-buffer-for-compute-shader/6
 */



#include "openglwindow.h"


//---------------------------------------------------------------------
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;
    format.setSamples(16);

    format.setMajorVersion(4);
    format.setMinorVersion(3);
    //format.setProfile(QSurfaceFormat::CoreProfile);
    //format.setOption(QSurfaceFormat::DebugContext);

    OpenGLWindow window;
    window.setFormat(format);
    window.resize(640, 480);
    window.show();

    window.setAnimating(true);

    return app.exec();


    //window.compute();

}

//---------------------------------------------------------------------


