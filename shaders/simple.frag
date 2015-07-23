#version 410

in vec3 f_normal;
in vec2 f_texcoord;

out vec4 outputColor;

void main() 
{
    //outputColor = vec4((normalize(f_normal) + 1.0f) * 0.5f, 1.0f);
    outputColor = vec4(f_texcoord, 0.0, 1.0);
}
