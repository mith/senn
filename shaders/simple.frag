#version 450 core

in vec3 world_position;
in vec4 shadow_position_near;
in vec4 shadow_position_far;
in vec3 world_normal;
in vec2 f_texcoord;

out vec4 outputColor;

uniform mat4 rotMat;
uniform sampler2D diffuse;
uniform sampler2DShadow shadowmap_near;
uniform sampler2DShadow shadowmap_far;
uniform vec3 directional_light;
uniform float light_intensity;
uniform vec3 camera_position;

vec3 lambert(vec3 color, vec3 normal, vec3 light_direction)
{
    return clamp(dot(normal, light_direction), 0.0, 1.0) * color;
}

vec3 blinn(vec3 color, vec3 normal, vec3 light_direction)
{
    vec3 viewdir = normalize(camera_position - world_position).xyz;

    vec3 hv = normalize(viewdir + light_direction);
    float si = pow(clamp(dot(normal, hv), 0.0, 1.0), 16.0);
    return color * si;
}

void main() 
{
    vec3 diffuse_col = texture(diffuse, f_texcoord).xyz;
    vec3 ambient = 0.02 * diffuse_col;
    vec3 light_color = light_intensity
                     * (lambert(diffuse_col, world_normal, directional_light)
                     + blinn(diffuse_col, world_normal, directional_light));
    if (abs(shadow_position_near.x - 0.5) < 0.49 && abs(shadow_position_near.y - 0.5) < 0.49 && abs(shadow_position_near.z - 0.5) < 0.49) {
        float shadowFactor_near = textureProj(shadowmap_near, shadow_position_near);
        outputColor = vec4(light_color * shadowFactor_near + ambient, 1.0);
        //outputColor.rgb = vec3(0.2);
    } else {
        float shadowFactor_far = textureProj(shadowmap_far, shadow_position_far);
        outputColor = vec4(light_color * shadowFactor_far + ambient, 1.0);
        //outputColor.rgb = vec3(0.8);
    }
    //outputColor.rgb = vec3(shadowFactor_near);
    outputColor.a = 1.0;
}
