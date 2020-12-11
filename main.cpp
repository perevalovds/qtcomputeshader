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
#include <QtGui/QGuiApplication>


//---------------------------------------------------------------------
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    //Surface - for create and maintaing OpenGL context
    ComputeSurface surface;
    surface.setup();

    //Compute shader
    QString shader_file = ":/shader/compute_shader.csh";
    ComputeShader shader;
    shader.setup(shader_file, &surface);

    //Buffer for computations
    //We use std430 in shader's code, so it looks like no need to align to float*4
    //BUT: later I got in troubles with "vec3" arrays aligning, 
    //so I think it's better nevertheless to std430 use vec4 and aligning to float*4 ....
    //at least until I will clearly keep in mind std430 details :)
    const int N = 23;
    float buf[N];
    for (int i=0; i<N; i++) {
        buf[i] = i;
    }
    qDebug() << "Input buffer: ";
    for (int i=0; i<N; i++) {
        qDebug() << "  " << buf[i];
    }

    ComputeBuffer buffer;
    buffer.setup(&surface);
    buffer.allocate(buf, sizeof(buf));

    //Compute
    //bind buffer
    buffer.bind_for_shader(0);

    //bind
    shader.begin();

    //set uniforms
    shader.program().setUniformValue("coeff", 0.5f);

    //compute
    shader.compute(N);

    //unbind
    shader.end();

    //Download result to CPU
    buffer.read_to_cpu(buf, sizeof(buf));

    qDebug() << "Output buffer: ";
    for (int i=0; i<N; i++) {
        qDebug() << "  " << buf[i];
    }

    return 0;
}

//---------------------------------------------------------------------


