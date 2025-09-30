#ifndef COMPONENT_H
#define COMPONENT_H

#include "PCH.h"

struct Node
{
    std::string name;

    Node() = default;
    Node(const std::string& name) : name(name) {}
};

struct Tile
{
    uint32_t idx, idy;
    Tile() = default;
    Tile(uint32_t x, uint32_t y) : idx(x), idy(y) {}
};

struct Transform
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transform():
        position({ 0.0f, 0.0f, 0.0f }),
        rotation({ 0.0f, 0.0f, 0.0f }),
        scale   ({ 0.0f, 0.0f, 0.0f })
    {

    }
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

struct Material
{
    std::string shader_id;

    glm::vec3 diffuseColor   {1.0f, 1.0f, 1.0f};  // base color
    glm::vec3 specularColor  {1.0f, 1.0f, 1.0f};  // highlight color
    float     shininess      {32.0f};             // specular exponent
    float     opacity        {1.0f};              // transparency

    unsigned int diffuseMap  = 0;  // OpenGL texture ID
    unsigned int specularMap = 0;  // OpenGL texture ID
    unsigned int normalMap   = 0;  // OpenGL texture ID

    Material() = default;

    Material(const glm::vec3& _diff, const glm::vec3& _spec, float _shin)
        : diffuseColor(_diff), specularColor(_spec), shininess(_shin)
    {

    }
};

#endif
