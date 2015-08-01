#version 410

in vec3 f_position;

layout(location = 0) out float depth;

void main() 
{
    depth = gl_FragCoord.z;
}
