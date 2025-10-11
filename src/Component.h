#ifndef COMPONENT_H
#define COMPONENT_H

#include "PCH.h"

struct Transform
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale   ;

    glm::mat4 world_mat;

    Transform():
        position({ 0.0f, 0.0f, 0.0f }),
        rotation({ 0.0f, 0.0f, 0.0f }),
        scale   ({ 1.0f, 1.0f, 1.0f })
    {

    }

    glm::mat4 get_local_mat4() const
    {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);

        // rotation order: Y (yaw), X (pitch), Z (roll) — you can change if needed
        glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1,0,0));
        glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0,1,0));
        glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0,0,1));
        glm::mat4 rotationMat = rotZ * rotY * rotX;

        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);

        return translation * rotationMat * scaling;
    }
};

struct RigidBody
{
    glm::vec3 velocity;
};

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

struct Agent
{
    glm::vec2 move_direction { 1.0f, 0.0f };

    float move_amount   = 0.1f;
    float move_duration = 0.5f;
    float move_elapsed  = 0.0f;

    Agent() = default;

    void update_move(float _dt, Transform& _transform)
    {
        move_elapsed += _dt;
        if (move_elapsed > 1.0f)
            move_elapsed = 0.0f;

        float _ease = ease_out_elastic(move_elapsed);

        _transform.position = glm::mix(glm::vec3(0.0f), { 0.0f, 5.0f, 0.0f} , _ease);
    }

    float ease_out_elastic(float t) {
        const float c4 = (2.0f * 3.14159265f) / 3.0f;
        if (t == 0)
            return 0;
        if (t == 1)
            return 1;
        return pow(2, -10 * t) * sin((t * 10 - 0.75f) * c4) + 1;
    }

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

struct Material
{
    std::string shader_id;

    glm::vec3 diffuseColor   {1.0f, 1.0f, 1.0f};  // base color
    glm::vec3 specularColor  {1.0f, 1.0f, 1.0f};  // highlight color
    float     shininess      {32.0f};             // specular exponent
    float     opacity        {1.0f};              // transparency

    bool depth_test  = true;
    bool depth_write = true;

    unsigned int diffuseMap  = 0;  // OpenGL texture ID
    unsigned int specularMap = 0;  // OpenGL texture ID
    unsigned int normalMap   = 0;  // OpenGL texture ID

    Material() = default;

    Material(const glm::vec3& _diff, const glm::vec3& _spec, float _shin)
        : diffuseColor(_diff), specularColor(_spec), shininess(_shin)
    {

    }
};

struct LightData
{
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
};

struct DirectionalLight
{
    glm::vec3 direction   = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::vec3 ambient     = glm::vec3(0.2f);
    glm::vec3 diffuse     = glm::vec3(0.5f);
    glm::vec3 specular    = glm::vec3(1.0f);

    LightData to_light_data() const
    {
        LightData data{};
        data.direction = direction;
        data.ambient   = ambient;
        data.diffuse   = diffuse;
        data.specular  = specular;
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
