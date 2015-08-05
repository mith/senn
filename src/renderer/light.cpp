#include "light.hpp"

#include <iostream>

#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

Shadowmap::Shadowmap(ShaderLoader* shader_loader, glm::ivec2 size, glm::vec3 direction)
    : size(size)
{ 
    proj_matrix = glm::ortho(-2.0f, 25.0f, -2.0f, 20.0f, -100.0f, 100.0f);
    set_direction(direction);
    glTextureStorage2D(depth_texture.name, 1, GL_DEPTH_COMPONENT16, size.x, size.y);
    glTextureParameteri(depth_texture.name, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(depth_texture.name, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTextureParameteri(depth_texture.name, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(depth_texture.name, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(depth_texture.name, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTextureParameteri(depth_texture.name, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

    framebuffer.set_depth_attachment(depth_texture);
    shader = shader_loader->load_shader(ShaderDescriptor{"shadowmap.vert",
                                                         "shadowmap.frag"});
    glNamedFramebufferDrawBuffer(framebuffer.get_name(), GL_NONE);
 
}

void Shadowmap::set_direction(glm::vec3 direction)
{
    view_matrix = glm::lookAt(direction,
                              glm::vec3(0.0f, 0.0f, 0.0f),
                              glm::vec3(0.0f, 1.0f, 0.0f));
}

DirectionalLight::DirectionalLight(ShaderLoader* shader_loader, glm::vec3 direction)
    : direction(direction)
    , intensity(1.0f)
    , shadowmap(shader_loader, {1024.0f, 1024.0f}, direction)
{
}

void DirectionalLight::set_direction(glm::vec3 direction)
{
    this->direction = direction;
    shadowmap.set_direction(direction);
}
