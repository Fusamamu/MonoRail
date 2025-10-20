#ifndef CAMERA_H
#define CAMERA_H

#include "PCH.h"
#include "Ray.h"

struct CameraData
{
    glm::mat4 projection;
    glm::mat4 view;
    alignas(16) glm::vec3 viewPos;
};

struct Camera
{
    glm::vec3 position {0.0f, 0.0f, 3.0f};
    glm::vec3 target   {0.0f, 0.0f, 0.0f}; //will remove?
    glm::vec3 front    {0.0f, 0.0f, -1.0f};
    glm::vec3 up       {0.0f, 1.0f, 0.0f};
    glm::vec3 right    {1.0f, 0.0f, 0.0f};

    float fov        = 35.0f;     // field of view in degrees
    float aspect     = 4.0f/3.0f; // width / height
    float near_plane = 0.1f;
    float far_plane  = 100.0f;

    float yaw   = -90.0f; // horizontal rotation
    float pitch = 0.0f;   // vertical rotation

    Camera()
    {
        std::cout << "Camera constructor" << std::endl;
    }

    glm::mat4 get_view_matrix() const
    {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 get_projection_matrix() const
    {
        return glm::perspective(glm::radians(fov), aspect, near_plane, far_plane);
    }

    CameraData get_camera_data() const
    {
        CameraData data{};
        data.projection = get_projection_matrix();
        data.view       = get_view_matrix();
        data.viewPos    = position;
        return data;
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

    void move_forward(float distance)  { position += front * distance; }
    void move_backward(float distance) { position -= front * distance; }
    void move_right(float distance)    { position += right * distance; }
    void move_left(float distance)     { position -= right * distance; }
    void move_up(float distance)       { position += up * distance; }
    void move_down(float distance)     { position -= up * distance; }

    // --- Mouse look ---
    void rotate(float yaw_offset, float pitch_offset)
    {
        yaw   += yaw_offset;
        pitch += pitch_offset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        update_vectors();
    }

    void update_vectors()
    {
        // Convert spherical coordinates to Cartesian
        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(f);

        // Recalculate right and up vectors
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f,1.0f,0.0f)));
        up    = glm::normalize(glm::cross(right, front));
    }

    void update_angles_from_vectors()
    {
        glm::vec3 f = glm::normalize(front);
        yaw   = glm::degrees(atan2(f.z, f.x));
        pitch = glm::degrees(asin(f.y));

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        up    = glm::normalize(glm::cross(right, front));
    }

    // Convert screen coordinates (pixels) to world ray
    Ray screen_point_to_ray(const glm::vec2& screen_pos, const glm::vec2& screen_size) const
    {
        // Normalize screen coords to NDC
        float x = (2.0f * screen_pos.x) / screen_size.x - 1.0f;
        float y = 1.0f - (2.0f * screen_pos.y) / screen_size.y;
        glm::vec4 ray_ndc(x, y, -1.0f, 1.0f);

        // Clip space -> view space
        glm::vec4 ray_eye = glm::inverse(get_projection_matrix()) * ray_ndc;
        ray_eye.z = -1.0f; ray_eye.w = 0.0f;

        // View space -> world space
        glm::vec3 ray_wor = glm::normalize(glm::vec3(glm::inverse(get_view_matrix()) * ray_eye));

        return { position, ray_wor };
    }
};

#endif
