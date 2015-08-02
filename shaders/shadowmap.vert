#version 450 core
layout(location = 0) in vec3 position;

uniform mat4 modelMat;
uniform mat4 perspMat;

void main()
{
    gl_Position = perspMat * modelMat * vec4(position, 1.0);
}
