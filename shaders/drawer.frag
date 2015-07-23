#version 450
in vec2 f_texcoord;

out vec4 outputColor;

uniform sampler2D tex;

void main() {
    vec4 col = texture(tex, f_texcoord);
    outputColor = col;
}
