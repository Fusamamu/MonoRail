#ifndef COMPONENT_H
#define COMPONENT_H

#include "PCH.h"

struct RigidBody
{
    glm::vec3 velocity;
};

struct Boid
{
    uint32_t id;
    Boid() = default;
};

struct ParentChild
{
    entt::entity parent = entt::null;
    std::vector<entt::entity> children;
};

struct Parent {
    entt::entity entity = entt::null;
};

struct Children {
    std::vector<entt::entity> entities;
};

struct AABB
{
    glm::vec3 min{0.0f}; // Minimum corner
    glm::vec3 max{0.0f}; // Maximum corner

    AABB() = default;

    AABB(const glm::vec3& _min, const glm::vec3& _max)
        : min(_min), max(_max)
    {

    }

    // Get the center of the AABB
    glm::vec3 get_center() const
    {
        return (min + max) * 0.5f;
    }

    // Get the extents (half-size) of the AABB
    glm::vec3 get_extents() const
    {
        return (max - min) * 0.5f;
    }

    // Expand the AABB to include a point
    void expand(const glm::vec3& point)
    {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    // Check if a point is inside the AABB
    bool contains(const glm::vec3& point) const
    {
        return (point.x >= min.x && point.x <= max.x) &&
               (point.y >= min.y && point.y <= max.y) &&
               (point.z >= min.z && point.z <= max.z);
    }

    // Check intersection with another AABB
    bool intersects(const AABB& other) const
    {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }
};

struct LightData
{
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
    alignas(16) glm::mat4 light_space_mat;
};

struct DirectionalLight
{
    bool cast_shadow = true;
    float orthographic_size = 20.0f;

    glm::vec3 position    = glm::vec3(30.0f, 30.0f, 30.0f);
    glm::vec3 target      = glm::vec3(0.0f);
    glm::vec3 direction   = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::vec3 ambient     = glm::vec3(0.2f);
    glm::vec3 diffuse     = glm::vec3(0.5f);
    glm::vec3 specular    = glm::vec3(1.0f);

    glm::mat4 get_view_matrix() const
    {
        glm::vec3 dir = glm::normalize(direction);

        glm::vec3 _dir = glm::normalize(target - position);

        return glm::lookAt(position, position + _dir, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::mat4 get_projection_matrix(float orthoSize = 100.0f, float nearPlane = 0.1f, float farPlane = 100.0f) const
    {
        return glm::ortho(-orthographic_size, orthographic_size, -orthographic_size, orthographic_size, nearPlane, farPlane);
    }

    glm::mat4 get_light_space_matrix() const
    {
        return get_projection_matrix() * get_view_matrix();
    }

    LightData to_light_data() const
    {
        LightData data{};
        data.direction = direction;
        data.ambient   = ambient;
        data.diffuse   = diffuse;
        data.specular  = specular;
        data.light_space_mat = get_light_space_matrix();
        return data;
    }
};

struct FogData
{
    alignas(16) glm::vec3 fogColor;  // 12 bytes + 4 bytes padding
    alignas(4)  float fogStart;      // 4 bytes
    alignas(4)  float fogEnd;        // 4 bytes
    alignas(4)  float fogDensity;    // 4 bytes
    alignas(4)  float pad;           // extra padding to make total size multiple of 16 bytes
};

#endif
