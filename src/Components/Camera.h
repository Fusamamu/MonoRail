#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"

namespace Component
{
    struct CameraData
    {
        glm::mat4 projection;
        glm::mat4 view;
        alignas(16) glm::vec3 viewPos;
    };

    class Camera
    {
    public:
        glm::vec3 position {0.0f,  0.0f,  3.0f};
        glm::vec3 target   {0.0f,  0.0f,  0.0f};
        glm::vec3 front    {0.0f,  0.0f, -1.0f};
        glm::vec3 up       {0.0f,  1.0f,  0.0f};
        glm::vec3 right    {1.0f,  0.0f,  0.0f};

        float fov            = 35.0f    ;
        float aspect         = 4.0f/3.0f;
        float near_plane     = 0.1f     ;
        float far_plane      = 100.0f   ;
        float yaw            = -90.0f   ;
        float pitch          = 0.0f     ;
        float focus_distance = 10.0f    ;
        float focus_range    = 5.0f     ;

        Camera();

        glm::mat4  get_view_matrix      () const;
        glm::mat4  get_projection_matrix() const;
        CameraData get_camera_data      () const;

        void move_forward     (float _distance);
        void move_backward    (float _distance);
        void move_right       (float _distance);
        void move_left        (float _distance);
        void move_up          (float _distance);
        void move_down        (float _distance);

        void camera_move_left (float _distance);
        void camera_move_right(float _distance);
        void camera_move_up   (float _distance);
        void camera_move_down (float _distance);

        void rotate(float _yaw_offset, float _pitch_offset);

        void update_vectors            ();
        void update_angles_from_vectors();

        Ray screen_point_to_ray(const glm::vec2& _screen_pos, const glm::vec2& _screen_size) const;
    };
}

#endif // CAMERA_H
