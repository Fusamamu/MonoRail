#include "Grid.h"

Grid::Grid(size_t width, size_t height)
    : m_width(width), m_height(height) {
    resize(width, height);
}

void Grid::resize(size_t width, size_t height) {
    m_width  = width;
    m_height = height;
    m_data.assign(width * height, entt::null);
}

entt::entity& Grid::at(size_t x, size_t y) {
    check_bounds(x, y);
    return m_data[y * m_width + x];
}

const entt::entity& Grid::at(size_t x, size_t y) const {
    check_bounds(x, y);
    return m_data[y * m_width + x];
}

void Grid::generate_tiles(entt::registry& _registry) {
    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
            entt::entity _e = _registry.create();

            Mesh* _tile_mesh = AssetManager::instance().get_first_mesh("tile");

            auto& _tile = _registry.emplace<Tile>(_e, Tile(x, y));

            auto& _node = _registry.emplace<Node>(_e, Node());
            _node.name = "tile";

            auto& _transform = _registry.emplace<Transform>(_e);
            _transform.position = glm::vec3(x, 0, y);

            auto& _aabb = _registry.emplace<AABB>(_e, AABB());
            _aabb.min = { -0.5f, -0.5f, -0.5f };
            _aabb.max = {  0.5f,  0.5f,  0.5f };

            auto& _material = _registry.emplace<Material>(_e);
            _material.shader_id = "toon";

            auto& _mesh_renderer = _registry.emplace<MeshRenderer>(_e);
            _mesh_renderer.load_mesh      (_tile_mesh);
            _mesh_renderer.set_buffer_data(_tile_mesh);

            at(x, y) = _e;
        }
    }
}

void Grid::generate_tiles_with_perlin(entt::registry& _registry) {
    Procgen::PerlinNoise noise(12345); // fixed seed for reproducibility

    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {

            // Generate Perlin noise value for this tile
            double nx = static_cast<double>(x) / m_width;
            double ny = static_cast<double>(y) / m_height;
            double value = noise.noise(nx * 5.0, ny * 5.0); // 0–1 range

            //double value = 0.5;

            // Pick mesh/material based on noise value
            std::string tile_type;
            if (value < 0.3) {
                tile_type = "water";
            } else if (value < 0.6) {
                tile_type = "grass";

                entt::entity _e = _registry.create();

                Mesh* _tile_mesh = AssetManager::instance().get_first_mesh("tile_pillar");

                auto& _tile = _registry.emplace<Tile>(_e, Tile(x, y));

                auto& _node = _registry.emplace<Node>(_e, Node());
                _node.name = tile_type;

                auto& _transform = _registry.emplace<Transform>(_e);
                _transform.position = glm::vec3(x, 0, y);

                auto& _aabb = _registry.emplace<AABB>(_e, AABB());
                _aabb.min = { -0.5f, -0.5f, -0.5f };
                _aabb.max = {  0.5f,  0.5f,  0.5f };

                auto& _material = _registry.emplace<Material>(_e);
                _material.shader_id = "phong"; // could also vary by type

                auto& _mesh_renderer = _registry.emplace<MeshRenderer>(_e);
                _mesh_renderer.load_mesh      (_tile_mesh);
                _mesh_renderer.set_buffer_data(_tile_mesh);

                at(x, y) = _e;

            } else {
                tile_type = "mountain";
            }


        }
    }
}

void Grid::print() const {
    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
            std::cout << (at(x, y) != entt::null ? "X " : ". ");
        }
        std::cout << "\n";
    }
}

void Grid::check_bounds(size_t x, size_t y) const {
    if (x >= m_width || y >= m_height) {
        throw std::out_of_range("Grid index out of bounds");
    }
}
