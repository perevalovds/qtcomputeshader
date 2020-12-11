# qtcomputeshader
Example of using compute shader in Qt without graphical rendering.

The compute shader processes array of 23 floats and writes it to another array. 
Qt reads both input and output arrays back to CPU and prints to console.

Example contains ComputeBuffer, ComputeShader and ComputeSurface helper classes 
to simplify working with compute shaders.

### Requirements

* OpenGL >= 4.3.
* Qt >= 5.14.

Code was tested on Windows, but contains no platform-specific code.

### Credits
The code is made using ideas from the following codes:
* The basis is "openglwindow" Qt example
* Qt and shader connection is based on https://forum.qt.io/topic/104448/about-buffer-for-compute-shader/6
* Compute shader text contains fragments from https://github.com/1DIce/Qt3D_compute_particles_cpp

Useful links:
* Khronos docs about shader buffers: https://www.khronos.org/opengl/wiki/Shader_Storage_Buffer_Object
* Qt QOpenGLBuffer sources which show me that I must use bind/unbind often, because this buffer really doesn't do it (but should...):
https://code.woboq.org/qt5/qtbase/src/gui/opengl/qopenglbuffer.cpp.html