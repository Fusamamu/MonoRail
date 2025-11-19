#ifndef RAYCAST_H
#define RAYCAST_H

#include "PCH.h"
#include "Components/Component.h"

struct Ray
{
    glm::vec3 origin;
    glm::vec3 dir; // Should be normalized
};

// Returns true if ray intersects AABB; fills t_min and t_max with intersection distances.
bool ray_intersects_aabb(const Ray& ray, const AABB& box, float& t_min, float& t_max);

// Creates a world-space ray from camera position and screen coordinates.
Ray create_ray_from_camera(
    const glm::vec3& cam_pos,
    const glm::mat4& view,
    const glm::mat4& proj,
    const glm::vec2& screen_pos,
    const glm::vec2& screen_size
);

entt::entity ray_cast_select_entity(
    entt::registry& registry,
    const Ray& ray,
    float& out_distance
);

#endif // RAYCAST_H
