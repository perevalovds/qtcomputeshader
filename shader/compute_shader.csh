#version 430

uniform float particleStep;
uniform float finalCollisionFactor;


//struct ParticleData
//{
//    vec4 pos;
//    vec4 dir;
//    vec4 color;
//};

//layout (std430, binding = 0) coherent buffer Values
//layout (std430, binding = 0) coherent buffer Values
//{
//    ParticleData particles[];
//};

//layout(std430, binding = 0) buffer Pos 
//{
//    vec4 pos[];
//};

//layout(std430, binding = 1) buffer Vel 
//{
//    vec4 vel [];
//};


layout(std430, binding = 0) buffer Buf 
{
    float buf[];
};


layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
//layout (local_size_x = 1024) in;

void main(void)
{
    uint id = gl_GlobalInvocationID.x;

    /*vec3 p = pos[id].xyz;
 
    p.x += 1.f;
    p.y += 0.f;
    p.z += -1.f;
 
    pos[id].xyz = p;*/
    buf[id] = id*2;
}
