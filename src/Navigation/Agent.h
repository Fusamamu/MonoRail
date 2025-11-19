#ifndef AGENT_H
#define AGENT_H

#include "Components/Component.h"
#include "Components/Transform.h"

namespace NAV
{
    class Agent
    {
    public:
        enum class FaceDirection
        {
            NORTH = 0,
            WEST  = 1,
            EAST  = 2,
            SOUTH = 3,
        };

        FaceDirection previous_face_direction = FaceDirection::NORTH;
        FaceDirection current_face_direction  = FaceDirection::NORTH;

        glm::vec2 move_direction { 1.0f, 0.0f };

        float move_amount   = 1.0f;
        float move_duration = 0.5f;
        float move_elapsed  = 0.0f;

        bool following_path = false;

        size_t previous_path_index = -1;
        size_t current_path_index  = 0;

        std::vector<glm::vec3> target_path;

        Agent() = default;
        ~Agent() = default;

        void update(Component::Transform& _transform, float _dt)
        {
            update_following_path(_dt, _transform);
        }

        void update_following_path(float _dt, Component::Transform& _transform)
        {
            if (!following_path || target_path.empty())
                return;

            glm::vec3 _previous_pos = _transform.position;
            if (previous_path_index != -1)
                _previous_pos = target_path[previous_path_index];
            glm::vec3 target_pos = target_path[current_path_index];

            glm::vec3 dir = target_pos - _transform.position;

            float dist = glm::length   (dir);

            glm::vec3 _dir_norm = glm::normalize(target_pos - _previous_pos);

            std::cout << "Direction norm : " << _dir_norm.x << ", " << _dir_norm.y << ", " << _dir_norm.z << std::endl;

            glm::vec3 _target_rotation = glm::vec3(0.0f);

            previous_face_direction = current_face_direction;

            if (_dir_norm.x > 0.0f)
            {
                current_face_direction = FaceDirection::EAST;
                _target_rotation = glm::vec3(0.0f, glm::radians(-90.0f), 0.0f);
            }
            else if (_dir_norm.x < 0.0f)
            {
                current_face_direction = FaceDirection::WEST;
                _target_rotation = glm::vec3(0.0f,  glm::radians(90.0f), 0.0f);
            }
            else if (_dir_norm.z > 0.0f)
            {
                current_face_direction = FaceDirection::SOUTH;
                 _target_rotation = glm::vec3(0.0f, glm::radians(180.0f), 0.0f);
            }
            else if (_dir_norm.z < 0.0f)
            {
                current_face_direction = FaceDirection::NORTH;
                _target_rotation = glm::vec3(0.0f, glm::radians(0.0f), 0.0f);
            }

            const float epsilon = 0.1f;
            if (dist < epsilon)
            {
                previous_path_index = current_path_index;
                current_path_index++;
                if (current_path_index >= target_path.size())
                {
                    following_path = false;
                    return;
                }

                target_pos = target_path[current_path_index];
                dir  = target_pos - _transform.position;
                dist = glm::length(dir);
            }

            if (dist > 0.0f)
                dir /= dist;


            float speed = move_amount / move_duration;
            _transform.position += dir * speed * _dt;

            float diff = glm::abs(glm::mod(_target_rotation.y - _transform.rotation.y + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>());

            // define a small threshold, e.g. 0.5 degrees in radians
            const float angle_epsilon = glm::radians(0.5f);

            if (diff > angle_epsilon)
            {
                auto lerp_angle = [](float a, float b, float t)
                {
                    float delta = glm::mod(b - a + glm::pi<float>(), glm::two_pi<float>()) - glm::pi<float>();
                    return a + delta * glm::clamp(t, 0.0f, 1.0f);
                };

                const float rotation_speed = 5.0f;
                _transform.rotation.y = lerp_angle(_transform.rotation.y, _target_rotation.y, rotation_speed * _dt);
            }
            else
            {
                _transform.rotation.y = _target_rotation.y;
            }


            move_elapsed += _dt;
        }

        void update_move(float _dt, Component::Transform& _transform)
        {
            move_elapsed += _dt;
            if (move_elapsed > 1.0f)
                move_elapsed = 0.0f;

            float _ease = ease_out_elastic(move_elapsed);

            _transform.position = glm::mix(glm::vec3(0.0f), { 0.0f, 5.0f, 0.0f} , _ease);
        }

        float ease_out_elastic(float t)
        {
            const float c4 = (2.0f * 3.14159265f) / 3.0f;
            if (t == 0)
                return 0;
            if (t == 1)
                return 1;
            return pow(2, -10 * t) * sin((t * 10 - 0.75f) * c4) + 1;
        }
    };
}

#endif
