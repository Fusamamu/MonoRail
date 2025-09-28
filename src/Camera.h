#ifndef CAMERA_H
#define CAMERA_H

#include "PCH.h"

struct Camera
{
    glm::vec3 position {0.0f, 0.0f, 3.0f};
    glm::vec3 target   {0.0f, 0.0f, 0.0f};
    glm::vec3 up       {0.0f, 1.0f, 0.0f};

    float fov        = 45.0f;     // field of view in degrees
    float aspect     = 4.0f/3.0f; // width / height
    float near_plane = 0.1f;
    float far_plane  = 100.0f;

    Camera() = default;

    glm::mat4 get_view_matrix() const
    {
        return glm::lookAt(position, target, up);
    }

    glm::mat4 get_projection_matrix() const
    {
        return glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
    }

    void camera_move_left(float distance)
    {
        glm::vec3 forward = glm::normalize(target - position);
        glm::vec3 right   = glm::normalize(glm::cross(forward, up));
        position -= right * distance;
        target   -= right * distance;
    }

    void camera_move_right(float distance)
    {
        glm::vec3 forward = glm::normalize(target - position);
        glm::vec3 right   = glm::normalize(glm::cross(forward, up));
        position += right * distance;
        target   += right * distance;
    }

    void camera_move_up(float distance)
    {
        glm::vec3 dir = glm::normalize(up);
        position += dir * distance;
        target   += dir * distance;
    }

    void camera_move_down(float distance)
    {
        glm::vec3 dir = glm::normalize(up);
        position -= dir * distance;
        target   -= dir * distance;
    }
};

#endif
