#pragma once

#define GLM_SWIZZLE
#include <glm/glm.hpp>

struct Camera {
    glm::vec3 position;
    glm::ivec2 size;
    float fovy;
    float yaw;
    float pitch;

    Camera()
        : position({ 0.0f, 0.0f, 0.0f })
        , yaw(0.0f)
        , pitch(0.0f)
    {
    }

    glm::mat4 get_projection_matrix();
    glm::mat4 get_view_matrix();
    glm::mat4 get_rot_matrix();

    void add_yaw(float n)
    {
        yaw += n;
    }

    void add_pitch(float n);

    void move(glm::vec3 d)
    {
        position += d;
    }

    void move_rotated(glm::vec3 d)
    {
        glm::vec4 d4 = get_rot_matrix() * glm::vec4(d.x, d.y, d.z, 1.0f);
        position += glm::vec3(d4.x, d4.y, d4.z);
    }
};

