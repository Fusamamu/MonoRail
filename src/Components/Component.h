#ifndef COMPONENT_H
#define COMPONENT_H

#include "PCH.h"

// struct Transform
// {
//     glm::vec3 position;
//     glm::vec3 rotation;
//     glm::vec3 scale   ;
//
//     glm::mat4 world_mat;
//
//     Transform():
//         position({ 0.0f, 0.0f, 0.0f }),
//         rotation({ 0.0f, 0.0f, 0.0f }),
//         scale   ({ 1.0f, 1.0f, 1.0f })
//     {
//
//     }
//
//     glm::mat4 get_local_mat4() const
//     {
//         glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
//
//         // rotation order: Y (yaw), X (pitch), Z (roll) — you can change if needed
//         glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1,0,0));
//         glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0,1,0));
//         glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0,0,1));
//         glm::mat4 rotationMat = rotZ * rotY * rotX;
//
//         glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);
//
//         return translation * rotationMat * scaling;
//     }
// };

struct RigidBody
{
    glm::vec3 velocity;
};

struct Node
{
    std::string name;

    bool is_active = false;
    bool is_dirty  = false;
    bool is_static = false;

    Node() = default;
    Node(const std::string& name) : name(name) {}
};

struct Tile
{
    uint32_t idx, idy;
    Tile() = default;
    Tile(uint32_t x, uint32_t y) : idx(x), idy(y) {}
};

struct NodeIndex
{
    int32_t idx;
    int32_t idy;
    int32_t idz;

    NodeIndex(int32_t _idx, int32_t _idy, int32_t _idz) : idx(_idx), idy(_idy), idz(_idz) {}
    NodeIndex() = default;
    ~NodeIndex() = default;

    NodeIndex operator+(const NodeIndex& _other) const{
        return NodeIndex(idx + _other.idx, idy + _other.idy, idz + _other.idz);
    }
    
    bool operator==(const NodeIndex& _other) const{
        return idx == _other.idx && idy == _other.idy && idz == _other.idz; 
    }

    friend std::ostream& operator<<(std::ostream& os, const NodeIndex& _other)
    {
        os << _other.idx << ", " << _other.idy << ", " << _other.idz;
        return os;
    }
};

namespace std
{
    template<>
    struct hash<NodeIndex>
    {
        std::size_t operator()(const NodeIndex& k) const noexcept
        {
            return (std::hash<int>()(k.idx) * 73856093) ^ (std::hash<int>()(k.idz) * 19349663);
        }
    };
}

enum class TileType : uint8_t
{
    NONE   = 0,
    GROUND = 1 << 0,
};

struct Node3D
{
    int32_t idx;
    int32_t idy;
    int32_t idz;

    uint8_t bit;

    TileType type = TileType::NONE;

    std::array<entt::entity, 8> corner_nodes;

    bool is_occupied = false;

    Node3D() = default;
    Node3D(uint32_t x, uint32_t y, uint32_t z)
        : idx(x), idy(y), idz(z) {}

    NodeIndex to_node_index() const{
        return NodeIndex(idx, idy, idz);
    }
    NodeIndex to_node_index(int32_t _idx, int32_t _idy, int32_t _idz){
        return NodeIndex(idx + _idx, idy + _idy, idz + _idz);
    }

    uint8_t to_bitmask(entt::registry& _registry)
    {
        uint8_t mask = 0;
        for (size_t i = 0; i < corner_nodes.size(); ++i)
        {
            if (corner_nodes[i] != entt::null)
            {
                Node3D _node = _registry.get<Node3D>(corner_nodes[i]);
                if (_node.is_occupied)
                    mask |= (1 << (7 - i));
            }
        }

        bit = mask;
        return mask;
    }

    void print() const{ std::cout << *this; }

    friend std::ostream& operator<<(std::ostream& os, const Node3D& node)
    {
        os << "Node3D("
           << node.idx << ", "
           << node.idy << ", "
           << node.idz << ") "
           << "occupied=" << std::boolalpha << node.is_occupied;

        uint8_t mask = 0;
        for (size_t i = 0; i < node.corner_nodes.size(); ++i)
        {
            if (node.corner_nodes[i] != entt::null) // occupied
                mask |= (1 << i);
        }

        os << " corner_mask=0b" << std::bitset<8>(mask);

        return os;
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

// struct Material
// {
//     std::string shader_id;
//
//     glm::vec4 rgba           { 1.0f, 1.0f, 1.0f, 1.0f };
//     glm::vec3 diffuse_color  {1.0f, 1.0f, 1.0f};  // base color
//     glm::vec3 specularColor  {1.0f, 1.0f, 1.0f};  // highlight color
//     float     shininess      {32.0f};             // specular exponent
//     float     opacity        {1.0f};              // transparency
//
//     bool cast_shadow = true;
//     bool depth_test  = true;
//     bool depth_write = true;
//
//     GLuint diffuseMap  = 0;  // OpenGL texture ID
//     GLuint specularMap = 0;  // OpenGL texture ID
//     GLuint normalMap   = 0;  // OpenGL texture ID
//
//     Material() = default;
//
//     Material(const glm::vec3& _diff, const glm::vec3& _spec, float _shin)
//         : diffuse_color(_diff), specularColor(_spec), shininess(_shin)
//     {
//
//     }
// };

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
    glm::vec3 direction   = glm::vec3(-0.2f, -1.0f, -0.3f);
    glm::vec3 ambient     = glm::vec3(0.2f);
    glm::vec3 diffuse     = glm::vec3(0.5f);
    glm::vec3 specular    = glm::vec3(1.0f);

    glm::mat4 get_view_matrix() const
    {
        glm::vec3 dir = glm::normalize(direction);
        return glm::lookAt(position, position + dir, glm::vec3(0.0f, 1.0f, 0.0f));
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
