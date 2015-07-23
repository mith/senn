#version 410
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;

uniform mat4 orientation;
uniform vec3 translation;
uniform vec3 scale;
uniform mat4 perspMat;

out vec3 f_normal;
out vec2 f_texcoord;

void main()
{
    vec4 pos = vec4(position, 1.0f)
             * orientation
             * vec4(scale, 1.0f) 
             + vec4(translation, 1.0f);
    gl_Position = pos * perspMat;
    f_normal = (vec4(normal, 1.0f) * orientation).xyz;
    f_texcoord = texcoord;
}
