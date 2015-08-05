#include "camera.hpp"

#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::get_rot_matrix()
{
    return glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);
}

glm::mat4 Camera::get_view_matrix()
{
    glm::vec4 forward = {0.0f, 0.0f, 1.0f, 1.0f};
    glm::vec3 look_dir = (get_rot_matrix() * forward).xyz();
    return glm::lookAt(position, position + look_dir, glm::vec3(0.0f, -1.0f, 0.0f));
}

glm::mat4 Camera::get_projection_matrix()
{
    return glm::infinitePerspective(fovy, (float)size.x/size.y, 0.5f);
}

void Camera::add_pitch(float n)
{
    pitch += n;
    if (pitch < -70.0f) {
        pitch = -70.0f;
    }
    else if (pitch > 70.0f) {
        pitch = 70.0;
    }
}
