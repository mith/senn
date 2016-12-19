#version 450 core
layout(location = 0) in vec3 position;

uniform mat4 projMat;

void main()
{
    gl_Position = projMat * vec4(position, 1.0);
}
