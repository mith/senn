#version 410

in vec3 f_position;
in vec3 f_normal;
in vec2 f_texcoord;

out vec4 outputColor;

uniform mat4 perspMat;
uniform sampler2D diffuse;
uniform vec3 directional_light;
uniform vec3 point_light;

vec3 lambert(vec3 color, vec3 normal, vec3 light_direction)
{
    return dot(normal, light_direction) * color;
}

vec3 blinn(vec3 color, vec3 normal, vec3 light_direction)
{
    vec3 viewdir = normalize(vec4(f_position, 1.0) * perspMat).xyz;
    vec3 hv = normalize(viewdir + light_direction);
    float si = pow(clamp(dot(normal, hv), 0.0, 1.0), 16.0);
    return color * si;
}

void main() 
{
    vec3 point_light_dir = normalize(vec4(point_light,1.0) * perspMat 
                         - vec4(f_position,1.0) * perspMat).xyz;

    vec3 vs_normal = (vec4(f_normal, 1.0) * perspMat).xyz;

    vec3 diffuse_col = texture(diffuse, f_texcoord).xyz;
    vec3 ambient = 0.0 * diffuse_col;
    vec3 light_color = lambert(diffuse_col, vs_normal, point_light_dir)
                     + blinn(diffuse_col, f_normal, point_light_dir);
    outputColor = vec4(light_color + ambient, 1.0);
    //outputColor = vec4((normalize(f_normal) + 1.0f) * 0.5f, 1.0f);
    //outputColor = vec4(f_texcoord, 0.0, 1.0);
    //outputColor = vec4(1.0, 0.0, 0.0, 1.0);
    //outputColor = vec4(gl_FragCoord.xyz / 512, 1.0);
}
