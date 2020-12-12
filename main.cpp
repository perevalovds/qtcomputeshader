/*
# qtcomputeshader
Denis Perevalov, github.com/perevalovds
See readme.
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

    //Set buffers for computation
    //As you see, we use array of 23 floats, together with std430 layout specifier 
    //in shader's buffer declaration.
    //Such array works and we are not required to align data to 4*float.
    //But nevertheless you should't use vec3 anyway in shader buffer, 
    //because it's aligned itself to 16 floats anyway!
    //See details here:
    //https://stackoverflow.com/questions/38172696/should-i-ever-use-a-vec3-inside-of-a-uniform-buffer-or-shader-storage-buffer-o

    const int N = 23;
    float input[N];
    float output[N];
    for (int i=0; i<N; i++) {
        input[i] = i;
        output[i] = -i;
    }

    ComputeBuffer input_buffer, output_buffer;

    input_buffer.setup(&surface);
    output_buffer.setup(&surface);

    input_buffer.allocate(input, sizeof(input));
    output_buffer.allocate(output, sizeof(output));

    //Bind buffers to shader
    input_buffer.bind_for_shader(0);
    output_buffer.bind_for_shader(1);


    //Compute
    shader.begin();
    shader.program().setUniformValue("coeff", 0.5f); //set uniforms
    shader.compute(N);
    shader.end();


    //Download input and resulted buffers to CPU
    //Clear to be sure we really read them
    for (int i=0; i<N; i++) {
        input[i] = 0;
        output[i] = 0;
    }

    input_buffer.read_to_cpu(input, sizeof(input));
    output_buffer.read_to_cpu(output, sizeof(output));

    qDebug() << "Buffers: ";
    for (int i=0; i<N; i++) {
        qDebug() << "  " << input[i] << " " << output[i];
    }

    return 0;
}

//---------------------------------------------------------------------


