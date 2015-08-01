#version 410
layout(location = 0) in vec3 position;

uniform mat4 modelMat;
uniform mat4 perspMat;

out vec3 f_position;

void main()
{
    gl_Position = perspMat * modelMat * vec4(position, 1.0);
}
