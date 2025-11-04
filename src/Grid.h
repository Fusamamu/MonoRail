#ifndef GRID_H
#define GRID_H

#include "Asset/AssetManager.h"
#include "Component.h"
#include "PCH.h"
#include "MeshRenderer.h"
#include "PerlinNoise.h"

class Grid {
public:
    Grid() = default;
    Grid(size_t width, size_t height);
    ~Grid() = default;

    void resize(size_t width, size_t height);

    size_t get_width () const { return m_width; }
    size_t get_height() const { return m_height; }

    entt::entity& at(size_t x, size_t y);
    const entt::entity& at(size_t x, size_t y) const;

    void generate_tiles(entt::registry& _registry);
    void generate_tiles_with_perlin(entt::registry& _registry);
    void print() const;

private:
    size_t m_width {0};
    size_t m_height{0};
    std::vector<entt::entity> m_data;

    void check_bounds(size_t x, size_t y) const;
};

#endif
