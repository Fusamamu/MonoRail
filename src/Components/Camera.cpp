#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"

namespace Component
{
    Camera::Camera()
    {
    }

    glm::mat4 Camera::get_view_matrix() const
    {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 Camera::get_projection_matrix() const
    {
        return glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
    }

    CameraData Camera::get_camera_data() const
    {
        CameraData data{};
        data.projection = get_projection_matrix();
        data.view       = get_view_matrix();
        data.viewPos    = position;
        return data;
    }

    void Camera::move_forward (float distance) { position += front * distance; }
    void Camera::move_backward(float distance) { position -= front * distance; }
    void Camera::move_right   (float distance) { position += right * distance; }
    void Camera::move_left    (float distance) { position -= right * distance; }
    void Camera::move_up      (float distance) { position += up    * distance; }
    void Camera::move_down    (float distance) { position -= up    * distance; }

    void Camera::camera_move_left(float distance)
    {
        glm::vec3 forward = glm::normalize(target - position);
        glm::vec3 right_v = glm::normalize(glm::cross(forward, up));
        position -= right_v * distance;
        target   -= right_v * distance;
    }

    void Camera::camera_move_right(float distance)
    {
        glm::vec3 forward = glm::normalize(target - position);
        glm::vec3 right_v = glm::normalize(glm::cross(forward, up));
        position += right_v * distance;
        target   += right_v * distance;
    }

    void Camera::camera_move_up(float distance)
    {
        glm::vec3 dir = glm::normalize(up);
        position += dir * distance;
        target   += dir * distance;
    }

    void Camera::camera_move_down(float distance)
    {
        glm::vec3 dir = glm::normalize(up);
        position -= dir * distance;
        target   -= dir * distance;
    }

    void Camera::rotate(float yaw_offset, float pitch_offset)
    {
        yaw   += yaw_offset;
        pitch += pitch_offset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        update_vectors();
    }

    void Camera::update_vectors()
    {
        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(f);

        right = glm::normalize(glm::cross(front, glm::vec3(0.0f,1.0f,0.0f)));
        up    = glm::normalize(glm::cross(right, front));
    }

    void Camera::update_angles_from_vectors()
    {
        glm::vec3 f = glm::normalize(front);
        yaw   = glm::degrees(atan2(f.z, f.x));
        pitch = glm::degrees(asin(f.y));

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        right = glm::normalize(glm::cross(front, glm::vec3(0.0f,1.0f,0.0f)));
        up    = glm::normalize(glm::cross(right, front));
    }

    Ray Camera::screen_point_to_ray(const glm::vec2& _screen_pos, const glm::vec2& _screen_size) const
    {
        float x = (2.0f * _screen_pos.x) / _screen_size.x - 1.0f;
        float y = 1.0f - (2.0f * _screen_pos.y) / _screen_size.y;
        glm::vec4 ray_ndc(x, y, -1.0f, 1.0f);

        glm::vec4 ray_eye = glm::inverse(get_projection_matrix()) * ray_ndc;
        ray_eye.z = -1.0f; ray_eye.w = 0.0f;

        glm::vec3 ray_wor = glm::normalize(glm::vec3(glm::inverse(get_view_matrix()) * ray_eye));

        return { position, ray_wor };
    }
}
