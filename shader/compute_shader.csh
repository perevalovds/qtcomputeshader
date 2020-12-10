#version 430

uniform float particleStep;
uniform float finalCollisionFactor;


//Buffer for processing
layout(std430, binding = 0) buffer Buf 
{
    float buf[];
};

//Example of using structure:
//struct ParticleData
//{
//    vec4 pos;
//    vec4 dir;
//    vec4 color;
//};

//layout (std430, binding = 0) coherent buffer Values
//{
//    ParticleData particles[];
//};


layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main(void)
{
    uint id = gl_GlobalInvocationID.x;
    
    float value = buf[id];  //read
    
    value += id*0.5f;            //change
    
    buf[id] = value;        //write    

    /*vec3 p = pos[id].xyz;
 
    p.x += 1.f;
    p.y += 0.f;
    p.z += -1.f;
 
    pos[id].xyz = p;*/
    
}
