#ifndef GRID3D_H
#define GRID3D_H

#include "PCH.h"
#include "ResourceManager.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "PerlinNoise.h"
#include "MMath.h"
#include "Ray.h"
#include "InputSystem.h"
#include "Camera.h"

struct TileAnim
{
    entt::entity entity;
    float elapsed  = 0.0f;
    float duration = 0.25f; // seconds
};

class Grid3D {
public:
    Grid3D(size_t width, size_t height, size_t depth);
    Grid3D() = default;
    ~Grid3D() = default;

    void init(size_t width, size_t height, size_t depth);
    void resize(size_t width, size_t height, size_t depth);

    size_t get_width()  const { return m_width;  }
    size_t get_height() const { return m_height; }
    size_t get_depth()  const { return m_depth;  }

    entt::entity&       at(size_t x, size_t y, size_t z);
    const entt::entity& at(size_t x, size_t y, size_t z) const;

    entt::entity&       node_at(size_t x, size_t y, size_t z);
    const entt::entity& node_at(size_t x, size_t y, size_t z) const;

    bool out_of_bounds(size_t x, size_t y, size_t z) const;
    bool is_occupied(entt::registry& _registry, size_t x, size_t y, size_t z);

    void create_tile_instance      (entt::registry& _registry);
    void create_corner_instance     (entt::registry& _registry);
    void generate_tiles            (entt::registry& _registry);
    void generate_tiles_with_perlin(entt::registry& _registry);
    void generate_corner_nodes     (entt::registry& _registry);

    void fill_tile_at_level(entt::registry& _registry, uint32_t _level);

    void store_corners_refs(entt::registry& _registry);
    void store_tile_refs   (entt::registry& _registry);

    void add_tile_at   (entt::registry& _registry, size_t x, size_t y, size_t z);
    void add_tile_above(entt::registry& _registry, size_t x, size_t y, size_t z);

    void select_tile_at(entt::registry& _registry, size_t x, size_t y, size_t z)
    {
        std::cout << "select_tile_at" << std::endl;

        MeshRenderer& _mesh_renderer = _registry.get<MeshRenderer>(m_tile_mesh_e);
        _mesh_renderer.update_instance_color(m_tile_instance_data, 1, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        //_mesh_renderer.update_all_instance_colors(m_tile_instance_data, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    //void update(entt::registry& _registry);
    void update(entt::registry &_registry, Camera _camera, InputSystem& _input_system);

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

    void update_tile_animations(entt::registry& reg, float dt)
    {
        for (auto it = active_tile_anims.begin(); it != active_tile_anims.end();)
        {
            it->elapsed += dt * 2.5f;

            float t        = glm::clamp(it->elapsed / it->duration, 0.0f, 1.0f);
            float smooth_t = EASE::ease_out_back(t); // or smoothstep, ease_in_out, etc.

            reg.get<Transform>(it->entity).scale = glm::vec3(smooth_t);

            if (t >= 1.0f)
                it = active_tile_anims.erase(it); // remove finished
            else
                ++it;
        }
    }

    inline size_t tile_index(size_t x, size_t y, size_t z) const {
        return z * (m_width * m_height) + y * m_width + x;
    }

    inline size_t corner_index(size_t x, size_t y, size_t z) const {
        return z * ((m_width + 1) * (m_height + 1)) + y * (m_width + 1) + x;
    }

    void check_bounds(size_t x, size_t y, size_t z) const;
};

#endif // GRID3D_H
