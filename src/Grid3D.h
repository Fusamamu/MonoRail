#ifndef GRID3D_H
#define GRID3D_H

#include "PCH.h"
#include "MMath.h"
#include "ResourceManager.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "PerlinNoise.h"
#include "Ray.h"
#include "Camera.h"
#include "InputSystem.h"
#include "TileTable.h"

static inline float heuristic_manhattan(NodeIndex _node_a, NodeIndex _node_b);

struct TileAnim
{
    entt::entity entity;
    float elapsed  = 0.0f;
    float duration = 0.25f; // seconds
};

class Grid3D {
public:
    enum class Mode
    {
        NONE        = 0,
        ADD_TILE    = 1,
        REMOVE_TILE = 2,
        MARK_TILE   = 3,
        ADD_AGENT   = 4,
    };

    struct TileData
    {
        std::string mesh_name;
        NodeIndex node_index;
        glm::vec3 position;
    };

    Mode mode = Mode::NONE;

    entt::entity start_e = entt::null;
    entt::entity dest_e  = entt::null;

    Grid3D(size_t width, size_t height, size_t depth);
    Grid3D() = default;
    ~Grid3D() = default;

    void init  (size_t width, size_t height, size_t depth);
    void resize(size_t width, size_t height, size_t depth);

    size_t get_width()  const { return m_width;  }
    size_t get_height() const { return m_height; }
    size_t get_depth()  const { return m_depth;  }

    entt::entity&       at     (size_t x, size_t y, size_t z);
    const entt::entity& at     (size_t x, size_t y, size_t z) const;
    entt::entity&       node_at(size_t x, size_t y, size_t z);
    const entt::entity& node_at(size_t x, size_t y, size_t z) const;

    uint8_t get_surrounding_bit               (entt::registry& _registry, NodeIndex _node_index);
    uint8_t get_surrounding_bitmask_4direction(entt::registry& _registry, NodeIndex _node_index);

    bool out_of_bounds(size_t x, size_t y, size_t z) const;
    bool out_of_bounds(NodeIndex _node_index) const;
    bool is_occupied(entt::registry& _registry, size_t x, size_t y, size_t z);
    bool is_occupied(entt::registry& _registry, NodeIndex _node_index);

    void create_tile_instance      (entt::registry& _registry);
    void create_corner_instance    (entt::registry& _registry);
    void generate_tiles            (entt::registry& _registry);
    void generate_tiles_with_perlin(entt::registry& _registry);
    void generate_corner_nodes     (entt::registry& _registry);

    void fill_tile_at_level(entt::registry& _registry, uint32_t _level);

    void store_corners_refs(entt::registry& _registry);
    void store_tile_refs   (entt::registry& _registry);

    void add_tile_at   (entt::registry& _registry, const std::string& _mesh, size_t x, size_t y, size_t z, glm::vec3 _position);
    void add_tile_above(entt::registry& _registry, const std::string& _mesh, size_t x, size_t y, size_t z, glm::vec3 _position);

    void select_tile_at(entt::registry& _registry, size_t x, size_t y, size_t z)
    {
        MeshRenderer& _mesh_renderer = _registry.get<MeshRenderer>(m_tile_mesh_e);
        _mesh_renderer.update_instance_color(m_tile_instance_data, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    void update(entt::registry &_registry, Camera _camera, InputSystem& _input_system);

    std::optional<std::vector<entt::entity>> find_path(entt::registry& __restrict, NodeIndex _start_node, NodeIndex _dest_node);
    std::optional<std::vector<entt::entity>> find_path(entt::registry& _registry);

    void print_layer(size_t z) const;
private:
    size_t m_width  {0};
    size_t m_height {0};
    size_t m_depth  {0};

    std::vector<entt::entity> m_data;
    std::vector<entt::entity> m_corner_data;

    std::vector<InstanceData> m_tile_instance_data;
    std::vector<InstanceData> m_corner_instance_data;

    std::vector<TileAnim> active_tile_anims;

    entt::entity m_tile_mesh_e = entt::null;

    void add_tile_animation(entt::registry& reg, entt::entity _tile_e)
    {
        reg.get<Transform>(_tile_e).scale = glm::vec3(0.0f);
        active_tile_anims.push_back({ _tile_e, 0.0f, 0.25f });
    }

    void update_tile_animations(entt::registry& _registry, float dt)
    {
        for (auto _it = active_tile_anims.begin(); _it != active_tile_anims.end();)
        {
            auto& _node      = _registry.get<Node>(_it->entity);
            auto& _transform = _registry.get<Transform>(_it->entity);

            _it->elapsed += dt * 2.5f;

            float t        = glm::clamp(_it->elapsed / _it->duration, 0.0f, 1.0f);
            float smooth_t = EASE::ease_out_back(t); // or smoothstep, ease_in_out, etc.

            _transform.scale     = glm::vec3(smooth_t);
            _transform.world_mat = _transform.get_local_mat4();

            if (t >= 1.0f)
            {
                _it = active_tile_anims.erase(_it); // remove finished
                _node.is_dirty = false;
            }
            else
                ++_it;
        }
    }

    inline size_t tile_index(size_t x, size_t y, size_t z) const {
        return z * (m_width * m_height) + y * m_width + x;
    }

    inline size_t tile_index(NodeIndex _node_index) const {
        return _node_index.idz * (m_width * m_height) + _node_index.idy * m_width + _node_index.idx;
    }

    inline NodeIndex tile_index_to_node_coord(size_t _tile_index)
    {
        NodeIndex _result;
        size_t rem  = _tile_index % (m_width * m_height);
        _result.idz = _tile_index / (m_width * m_height);
        _result.idy = rem / m_width;
        _result.idx = rem % m_width;
        return _result;
    }

    inline size_t corner_index(size_t x, size_t y, size_t z) const {
        return z * ((m_width + 1) * (m_height + 1)) + y * (m_width + 1) + x;
    }

    void check_bounds(size_t x, size_t y, size_t z) const;
};

#endif // GRID3D_H
