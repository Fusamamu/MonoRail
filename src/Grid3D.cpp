#include "Grid3D.h"

Grid3D::Grid3D(size_t width, size_t height, size_t depth)
    : m_width(width), m_height(height), m_depth(depth) {
    resize(width, height, depth);
}

void Grid3D::resize(size_t width, size_t height, size_t depth)
{
    m_width  = width;
    m_height = height;
    m_depth  = depth;
    m_data.assign(width * height * depth, entt::null);
}

void Grid3D::update(entt::registry &_registry)
{
    update_tile_animations(_registry, 0.016f);
}

entt::entity& Grid3D::at(size_t x, size_t y, size_t z)
{
    check_bounds(x, y, z);
    return m_data[index(x, y, z)];
}

const entt::entity& Grid3D::at(size_t x, size_t y, size_t z) const
{
    check_bounds(x, y, z);
    return m_data[index(x, y, z)];
}

void Grid3D::generate_tiles(entt::registry& _registry)
{
    for (size_t z = 0; z < m_depth; ++z) {
        for (size_t y = 0; y < m_height; ++y) {
            for (size_t x = 0; x < m_width; ++x) {

                entt::entity _e = _registry.create();

                Mesh* _tile_mesh = ResourceManager::instance().get_first_mesh("base");

                auto& _node      = _registry.emplace<Node>     (_e);
                auto& _tile      = _registry.emplace<Tile3D>   (_e, Tile3D(x, y, z));
                auto& _transform = _registry.emplace<Transform>(_e);
                auto& _aabb      = _registry.emplace<AABB>     (_e);

                _node     .name     = "tile";
                _transform.position = glm::vec3(x, y, z);

                _aabb.min = { -0.5f, -0.5f, -0.5f };
                _aabb.max = {  0.5f,  0.5f,  0.5f };

                if (y == 0)
                {
                    _node.is_active   = true;
                    _tile.is_occupied = true;

                    auto& _material = _registry.emplace<Material>(_e);
                    _material.shader_id = "phong";

                    auto& _mesh_renderer = _registry.emplace<MeshRenderer>(_e);
                    _mesh_renderer.load_mesh      (_tile_mesh);
                    _mesh_renderer.set_buffer_data(_tile_mesh);
                }

                at(x, y, z) = _e;
            }
        }
    }
}

void Grid3D::generate_tiles_with_perlin(entt::registry& _registry) {
    PerlinNoise noise(12345); // fixed seed

    for (size_t z = 0; z < m_depth; ++z) {
        for (size_t y = 0; y < m_height; ++y) {
            for (size_t x = 0; x < m_width; ++x) {

                // Normalized coordinates
                double nx = static_cast<double>(x) / m_width;
                double ny = static_cast<double>(y) / m_height;
                double nz = static_cast<double>(z) / m_depth;

                // 3D Perlin noise
                //double value = noise.noise(nx * 3.0, ny * 3.0, nz * 3.0);
                double value = 0.0;

                // Threshold to decide block type
                std::string tile_type;
                if (value < 0.3) {
                    tile_type = "water";
                } else if (value < 0.6) {
                    tile_type = "grass";
                } else {
                    tile_type = "mountain";
                }

                entt::entity _e = _registry.create();

                std::string mesh_name = "tile";
                Mesh* _tile_mesh = ResourceManager::instance().get_first_mesh(mesh_name);

                auto& _tile = _registry.emplace<Tile3D>(_e, Tile3D(x, y, z));

                auto& _node = _registry.emplace<Node>(_e);
                _node.name = tile_type;

                auto& _transform = _registry.emplace<Transform>(_e);
                _transform.position = glm::vec3(x, y, z);

                auto& _aabb = _registry.emplace<AABB>(_e);
                _aabb.min = { -0.5f, -0.5f, -0.5f };
                _aabb.max = {  0.5f,  0.5f,  0.5f };

                auto& _material = _registry.emplace<Material>(_e);
                _material.shader_id = "phong";

                if (y == 0)
                {
                    auto& _mesh_renderer = _registry.emplace<MeshRenderer>(_e);
                    _mesh_renderer.load_mesh      (_tile_mesh);
                    _mesh_renderer.set_buffer_data(_tile_mesh);
                }

                at(x, y, z) = _e;
            }
        }
    }
}

bool Grid3D::is_occupied(entt::registry& _registry, size_t _x, size_t _y, size_t _z)
{
    auto& _e = at(_x, _y, _z);
    if (auto* _tile = _registry.try_get<Tile3D>(_e))
        return _tile->is_occupied;
    return false;
}

void Grid3D::add_tile_above(entt::registry& _registry, size_t x, size_t y, size_t z)
{
    add_tile_at(_registry, x, y + 1, z);
}

void Grid3D::add_tile_at(entt::registry& _registry, size_t x, size_t y, size_t z)
{
    if (out_of_bounds(x, y, z))
        return;
    if (is_occupied(_registry, x, y, z))
    {
        std::cout << "Cannot add tile at " << x << ", " << y << ", " << z << std::endl;
        return;
    }

    std::cout << " add tile at " << x << ", " << y << ", " << z << std::endl;

    entt::entity _e = _registry.create();

    Mesh* _tile_mesh = ResourceManager::instance().get_first_mesh("base");

    auto& _node      = _registry.emplace<Node>     (_e);
    auto& _tile      = _registry.emplace<Tile3D>   (_e, Tile3D(x, y, z));
    auto& _transform = _registry.emplace<Transform>(_e);
    auto& _aabb      = _registry.emplace<AABB>     (_e);

    _node.name        = "tile";
    _node.is_active   = true;
    _tile.is_occupied = true;
    _transform.position = glm::vec3(x, y, z);

    _aabb.min = { -0.5f, -0.5f, -0.5f };
    _aabb.max = {  0.5f,  0.5f,  0.5f };

    auto& _material = _registry.emplace<Material>(_e);
    _material.shader_id = "phong";

    auto& _mesh_renderer = _registry.emplace<MeshRenderer>(_e);
    _mesh_renderer.load_mesh      (_tile_mesh);
    _mesh_renderer.set_buffer_data(_tile_mesh);

    add_tile_animation(_registry, _e);

    at(x, y, z) = _e;
}

bool Grid3D::out_of_bounds(size_t x, size_t y, size_t z) const
{
    if (x >= m_width || y >= m_height || z >= m_depth)
        return true;
    return false;
}

void Grid3D::print_layer(size_t z) const
{
    if (z >= m_depth) {
        std::cerr << "Invalid layer index: " << z << "\n";
        return;
    }

    std::cout << "Layer " << z << ":\n";
    for (size_t y = 0; y < m_height; ++y) {
        for (size_t x = 0; x < m_width; ++x) {
            std::cout << (at(x, y, z) != entt::null ? "X " : ". ");
        }
        std::cout << "\n";
    }
}

void Grid3D::check_bounds(size_t x, size_t y, size_t z) const
{
    if (x >= m_width || y >= m_height || z >= m_depth)
    {
        throw std::out_of_range("Grid3D index out of bounds");
    }
}
