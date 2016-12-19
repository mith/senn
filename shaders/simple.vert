#version 450 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 modelMat;
uniform mat4 perspMat;
uniform mat4 shadowMat_near;
uniform mat4 shadowMat_far;
uniform mat4 rotMat;

out vec3 world_position;
out vec4 shadow_position_near;
out vec4 shadow_position_far;
out vec3 world_normal;
out vec2 f_texcoord;

void main()
{
    vec4 pos = modelMat * vec4(position, 1.0);
    world_position = pos.xyz;
    shadow_position_near = shadowMat_near * pos;
    shadow_position_far = shadowMat_far * pos;
    gl_Position = perspMat * pos;
    world_normal = (rotMat * vec4(normal, 1.0)).xyz;
    f_texcoord = texcoord;
}
