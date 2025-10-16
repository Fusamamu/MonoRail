#ifndef GRID3D_H
#define GRID3D_H

#include "ResourceManager.h"
#include "Component.h"
#include "PCH.h"
#include "MeshRenderer.h"
#include "PerlinNoise.h"
#include "MMath.h"

struct TileAnim
{
    entt::entity entity;
    float elapsed  = 0.0f;
    float duration = 0.25f; // seconds
};

class Grid3D {
public:
    Grid3D() = default;
    Grid3D(size_t width, size_t height, size_t depth);
    ~Grid3D() = default;

    void resize(size_t width, size_t height, size_t depth);

    size_t get_width()  const { return m_width;  }
    size_t get_height() const { return m_height; }
    size_t get_depth()  const { return m_depth;  }

    entt::entity&       at(size_t x, size_t y, size_t z);
    const entt::entity& at(size_t x, size_t y, size_t z) const;

    void generate_tiles            (entt::registry& _registry);
    void generate_tiles_with_perlin(entt::registry& _registry);

    void add_tile_at   (entt::registry& _registry, size_t x, size_t y, size_t z);
    void add_tile_above(entt::registry& _registry, size_t x, size_t y, size_t z);

    bool out_of_bounds(size_t x, size_t y, size_t z) const;

    void print_layer(size_t z) const;

    bool is_occupied(entt::registry& _registry, size_t x, size_t y, size_t z);

    void update(entt::registry& _registry);

private:
    size_t m_width  {0};
    size_t m_height {0};
    size_t m_depth  {0};

    std::vector<entt::entity> m_data;

    std::vector<TileAnim> active_tile_anims;

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

    inline size_t index(size_t x, size_t y, size_t z) const {
        return z * (m_width * m_height) + y * m_width + x;
    }

    void check_bounds(size_t x, size_t y, size_t z) const;
};

#endif // GRID3D_H
