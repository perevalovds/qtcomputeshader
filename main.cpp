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

#include "computesurface.h"


//---------------------------------------------------------------------
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    //Shader
    QString shader_file = ":/shader/compute_shader.csh";
    ComputeSurface surface;
    surface.setup(shader_file);

    //Prepare input array
    const int N = 23;
    float buf[N];
    for (int i=0; i<N; i++) {
        buf[i] = i;
    }

    qDebug() << "Input buffer: ";
    for (int i=0; i<N; i++) {
        qDebug() << "  " << buf[i];
    }

    //Upload to GPU
    ComputeBuffer buffer;
    buffer.setup(&surface);
    buffer.allocate(buf, sizeof(buf));

    //Compute
    buffer.bind_for_shader(0);
    surface.compute(N);

    //Download result to CPU
    buffer.read_to_cpu(buf, sizeof(buf));

    qDebug() << "Output buffer: ";
    for (int i=0; i<N; i++) {
        qDebug() << "  " << buf[i];
    }

    return 0;
}

//---------------------------------------------------------------------


