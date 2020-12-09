/*
# qtcomputeshader
Minimalistic example of using compute shader in Qt without graphical rendering.

The compute shader processes array of 23 floats and Qt reads it back and print to console. :)

### Requirements

* OpenGL >= 4.3.
* Qt >= 5.14.

Code was tested on Windows, but contains no platform-specific code.

### Credits
The code is made using ideas from the following codes:
* The basis is "openglwindow" Qt example
* Qt and shader connection is based on https://forum.qt.io/topic/104448/about-buffer-for-compute-shader/6
* Compute shader text contains fragments from https://github.com/1DIce/Qt3D_compute_particles_cpp

Denis Perevalov, github.com/perevalovds
 */

#include "glsurface.h"


//---------------------------------------------------------------------
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);


    OpenGLWindow window;

    window.compute();

    return 0;
}

//---------------------------------------------------------------------


