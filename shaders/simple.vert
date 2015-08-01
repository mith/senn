#version 410
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 modelMat;
uniform mat4 perspMat;
uniform mat4 shadowMat;
uniform mat4 rotMat;

out vec3 f_position;
out vec4 shadow_position;
out vec3 f_normal;
out vec2 f_texcoord;

void main()
{
    vec4 pos = modelMat * vec4(position, 1.0);
    f_position = pos.xyz;
    shadow_position = shadowMat * pos;
    gl_Position = perspMat * pos;
    f_normal = (rotMat * vec4(normal, 1.0)).xyz;
    f_texcoord = texcoord;
}
