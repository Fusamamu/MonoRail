#include "Ray.h"
#include "Components/Component.h"

bool ray_intersects_aabb(const Ray& ray, const AABB& box, float& t_min, float& t_max)
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

            t_min = std::max(t_min, t1);
            t_max = std::min(t_max, t2);

            if (t_min > t_max)
                return false;
        }
        else
        {
            // Ray parallel to slab, and origin not inside the box
            if (ray.origin[i] < box.min[i] || ray.origin[i] > box.max[i])
                return false;
        }
    }

    return true;
}

Ray create_ray_from_camera(
    const glm::vec3& cam_pos,
    const glm::mat4& view,
    const glm::mat4& proj,
    const glm::vec2& screen_pos,
    const glm::vec2& screen_size
)
{
    // Convert screen position to Normalized Device Coordinates (-1 to +1)
    float x = (2.0f * screen_pos.x) / screen_size.x - 1.0f;
    float y = 1.0f - (2.0f * screen_pos.y) / screen_size.y;
    glm::vec4 ray_ndc(x, y, -1.0f, 1.0f);

    // Clip space -> view space
    glm::vec4 ray_eye = glm::inverse(proj) * ray_ndc;
    ray_eye.z = -1.0f;
    ray_eye.w = 0.0f;

    // View space -> world space
    glm::vec3 ray_wor = glm::normalize(glm::vec3(glm::inverse(view) * ray_eye));

    return { cam_pos, ray_wor };
}

entt::entity ray_cast_select_entity(
    entt::registry& registry,
    const Ray& ray,
    float& out_distance
)
{
    entt::entity closest_entity = entt::null;
    float closest_t = std::numeric_limits<float>::max();

    // Include Node in the view
    auto _view = registry.view<Node, Transform, AABB>();

    for (auto entity : _view)
    {
        auto [node, transform, box] = _view.get<Node, Transform, AABB>(entity);

        if (!node.is_active)
            continue;

        // Transform AABB into world space
        glm::vec3 world_min = box.min + transform.position;
        glm::vec3 world_max = box.max + transform.position;
        AABB world_box{ world_min, world_max };

        float t_min, t_max;
        if (ray_intersects_aabb(ray, world_box, t_min, t_max))
        {
            if (t_min < closest_t && t_min >= 0.0f)
            {
                closest_t      = t_min;
                closest_entity = entity;
            }
        }
    }

    out_distance = closest_t;
    return closest_entity;
}

