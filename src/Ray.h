#ifndef RAYCAST_H
#define RAYCAST_H

#include "PCH.h"
#include "Component.h"

// Ray structure
struct Ray
{
    glm::vec3 origin;
    glm::vec3 dir; // Should be normalized
};

// Ray-AABB intersection using the "slab method"
inline bool ray_intersects_aabb(const Ray& ray, const AABB& box, float& t_min, float& t_max)
{
    t_min = 0.0f;
    t_max = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; ++i)
    {
        if (ray.dir[i] != 0.0f)
        {
            float t1 = (box.min[i] - ray.origin[i]) / ray.dir[i];
            float t2 = (box.max[i] - ray.origin[i]) / ray.dir[i];

            if (t1 > t2) std::swap(t1, t2);

            t_min = t1 > t_min ? t1 : t_min;
            t_max = t2 < t_max ? t2 : t_max;

            if (t_min > t_max) return false;
        }
        else
        {
            // Ray parallel to slab, and origin not inside
            if (ray.origin[i] < box.min[i] || ray.origin[i] > box.max[i])
                return false;
        }
    }

    return true;
}

// Helper: create a ray from camera
inline Ray create_ray_from_camera(const glm::vec3& cam_pos, const glm::mat4& view, const glm::mat4& proj, const glm::vec2& screen_pos, const glm::vec2& screen_size)
{
    // Convert screen position to NDC (-1 to 1)
    float x = (2.0f * screen_pos.x) / screen_size.x - 1.0f;
    float y = 1.0f - (2.0f * screen_pos.y) / screen_size.y;
    glm::vec4 ray_ndc(x, y, -1.0f, 1.0f);

    // Clip space -> view space
    glm::vec4 ray_eye = glm::inverse(proj) * ray_ndc;
    ray_eye.z = -1.0f; ray_eye.w = 0.0f;

    // View space -> world space
    glm::vec3 ray_wor = glm::normalize(glm::vec3(glm::inverse(view) * ray_eye));

    return { cam_pos, ray_wor };
}

#endif
