#include "light.hpp"

#include <iostream>

#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shadowmap::Shadowmap(ShaderLoader* shader_loader, glm::ivec2 size, glm::vec3 direction)
    : size(size)
{ 
    proj_matrix = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, -60.0f, 40.0f);
    set_direction(direction, glm::vec3(0.0f, 0.0f, 0.0f), {0.0f, 1.0f, 0.0f});
    glTextureStorage2D(depth_texture.name, 1, GL_DEPTH_COMPONENT16, size.x, size.y);
    glTextureParameteri(depth_texture.name, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(depth_texture.name, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTextureParameteri(depth_texture.name, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTextureParameteri(depth_texture.name, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glm::vec4 border_color = {1.0f, 1.0f, 1.0f, 1.0f};
    glTextureParameterfv(depth_texture.name, GL_TEXTURE_BORDER_COLOR, (const GLfloat*)&border_color);
    glTextureParameteri(depth_texture.name, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTextureParameteri(depth_texture.name, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    framebuffer.set_depth_attachment(depth_texture);
    shader = shader_loader->load_shader(ShaderDescriptor{"shadowmap.vert",
                                                         "shadowmap.frag"});
    glNamedFramebufferDrawBuffer(framebuffer.get_name(), GL_NONE);
 
}

void Shadowmap::set_direction(glm::vec3 direction, glm::vec3 offset, glm::vec3 up)
{
    view_matrix = glm::lookAt(offset,
                              offset - direction,
                              glm::vec3(up));
}

void Shadowmap::set_extents(glm::vec2 x, glm::vec2 y, glm::vec2 z)
{
    proj_matrix = glm::ortho(x[0], x[1], y[0], y[1], z[0], z[1]);
}

DirectionalLight::DirectionalLight(ShaderLoader* shader_loader, glm::vec3 direction)
    : direction(direction)
    , intensity(0.8f)
    , shadowmap(shader_loader, {1024.0f, 1024.0f}, direction)
{
}

void DirectionalLight::set_direction(glm::vec3 direction)
{
    this->direction = direction;
    shadowmap.set_direction(direction, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
}
