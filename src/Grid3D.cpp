#include "Grid3D.h"

Grid3D::Grid3D(size_t width, size_t height, size_t depth)
    : m_width(width), m_height(height), m_depth(depth)
{
    resize(width, height, depth);
}

void Grid3D::init(size_t width, size_t height, size_t depth)
{
    resize(width, height, depth);
}

void Grid3D::resize(size_t width, size_t height, size_t depth)
{
    m_width  = width;
    m_height = height;
    m_depth  = depth;
    m_data       .assign(width * height * depth, entt::null);
    m_corner_data.assign((width + 1) * (height + 1) * (depth + 1), entt::null);
}

void Grid3D::update(entt::registry &_registry)
{
    update_tile_animations(_registry, 0.016f);
}

entt::entity& Grid3D::at(size_t x, size_t y, size_t z)
{
    check_bounds(x, y, z);
    return m_data[tile_index(x, y, z)];
}

const entt::entity& Grid3D::at(size_t x, size_t y, size_t z) const
{
    check_bounds(x, y, z);
    return m_data[tile_index(x, y, z)];
}

entt::entity& Grid3D::node_at(size_t x, size_t y, size_t z)
{
    //check_bounds(x, y, z);
    return m_corner_data[corner_index(x, y, z)];
}

const entt::entity& Grid3D::node_at(size_t x, size_t y, size_t z) const
{
    //check_bounds(x, y, z);
    return m_corner_data[corner_index(x, y, z)];
}

void Grid3D::generate_tiles(entt::registry& _registry)
{
    m_tile_instance_data.resize(m_data.size());

    for (size_t z = 0; z < m_depth; ++z) {
        for (size_t y = 0; y < m_height; ++y) {
            for (size_t x = 0; x < m_width; ++x) {

                entt::entity _e = _registry.create();

                auto& _node         = _registry.emplace<Node>     (_e);
                auto& _tile= _registry.emplace<Node3D>   (_e, Node3D(x, y, z));
                auto& _transform    = _registry.emplace<Transform>(_e);
                auto& _aabb         = _registry.emplace<AABB>     (_e);

                _node.name        = "tile";
                _node.is_active   = true;
                _tile.is_occupied = true;

                _transform.position = glm::vec3(x, y, z);
                _transform.scale    = glm::vec3(0.2f);

                _aabb.min = { -0.5f, -0.5f, -0.5f };
                _aabb.max = {  0.5f,  0.5f,  0.5f };

                InstanceData _instance_data;
                _instance_data.model = _transform.get_local_mat4();
                _instance_data.color = glm::vec4(0.0f, 1.0f, 0.0f, 0.05f);

                m_tile_instance_data.push_back(_instance_data);

                at(x, y, z) = _e;
            }
        }
    }
}

void Grid3D::create_tile_instance(entt::registry& _registry)
{
    auto _e = _registry.create();

    Mesh* _p_mesh = ResourceManager::instance().get_first_mesh("bevel_cube");

    auto& _node = _registry.emplace<Node>(_e, Node());
    _node.name = "cube grid";

    auto& _transform = _registry.emplace<Transform>(_e);
    _transform.position = glm::vec3(0.0f);

    auto& _aabb = _registry.emplace<AABB>(_e, AABB());
    _aabb.min = { -0.5f, -0.5f, -0.5f };
    _aabb.max = {  0.5f,  0.5f,  0.5f };

    Material _material;
    _material.shader_id = "object_instance";
    _material.rgba      = glm::vec4(0.0f, 1.0f, 0.0f, 0.1f);

    auto& _material_comp = _registry.emplace<Material>(_e);
    _material_comp.shader_id   = _material.shader_id;
    _material_comp.depth_test  = _material.depth_test;
    _material_comp.depth_write = _material.depth_write;
    _material_comp.diffuseMap  = _material.diffuseMap;
    _material_comp.rgba        = _material.rgba;

    auto& _mesh_renderer = _registry.emplace<MeshRenderer>(_e);
    _mesh_renderer.load_mesh      (_p_mesh);
    _mesh_renderer.set_buffer_data(_p_mesh);

    _mesh_renderer.use_instancing = true;
    _mesh_renderer.instance_count = m_tile_instance_data.size();
    _mesh_renderer.set_instance_data(m_tile_instance_data);
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

                auto& _tile = _registry.emplace<Node3D>(_e, Node3D(x, y, z));

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

void Grid3D::generate_corner_nodes(entt::registry& _registry)
{
    m_corner_instance_data.reserve(m_corner_data.size());

    for (size_t z = 0; z < m_depth + 1; ++z) {
        for (size_t y = 0; y < m_height + 1; ++y) {
            for (size_t x = 0; x < m_width + 1; ++x) {

                entt::entity _e = _registry.create();

                auto& _node      = _registry.emplace<Node>     (_e);
                auto& _tile      = _registry.emplace<Node3D>   (_e, Node3D(x, y, z));
                auto& _transform = _registry.emplace<Transform>(_e);
                auto& _aabb      = _registry.emplace<AABB>     (_e);

                _node.name        = "corner_node";
                _node.is_active   = true;
                _tile.is_occupied = true;

                _transform.position = glm::vec3(x, y, z) - glm::vec3(0.5f);
                _transform.scale    = glm::vec3(0.1f, 0.1f, 0.1f);

                _aabb.min = { -0.1f, -0.1f, -0.1f };
                _aabb.max = {  0.1f,  0.1f,  0.1f };

                InstanceData _instance_data;
                _instance_data.model = _transform.get_local_mat4();
                _instance_data.color = glm::vec4(0.1f, 1.0f, 1.0f, 0.1f);

                m_corner_instance_data.push_back(_instance_data);

                node_at(x, y, z) = _e;
            }
        }
    }
}

void Grid3D::create_corner_instance(entt::registry& _registry)
{
    entt::entity _e = _registry.create();

    Mesh* _p_mesh = ResourceManager::instance().get_first_mesh("sphere");

    auto& _node = _registry.emplace<Node>(_e, Node());
    _node.name = "corner_grid";

    auto& _transform = _registry.emplace<Transform>(_e);
    _transform.position = glm::vec3(0.0f);

    auto& _aabb = _registry.emplace<AABB>(_e, AABB());
    _aabb.min = { -0.5f, -0.5f, -0.5f };
    _aabb.max = {  0.5f,  0.5f,  0.5f };

    Material _material;
    _material.shader_id     = "object_instance";
    _material.rgba          = glm::vec4(1.0f, 0.0f, 0.0f, 0.1f);

    auto& _material_comp = _registry.emplace<Material>(_e);
    _material_comp.shader_id   = _material.shader_id;
    _material_comp.depth_test  = _material.depth_test;
    _material_comp.depth_write = _material.depth_write;
    _material_comp.diffuseMap  = _material.diffuseMap;
    _material_comp.rgba        = _material.rgba;

    auto& _mesh_renderer = _registry.emplace<MeshRenderer>(_e);
    _mesh_renderer.load_mesh      (_p_mesh);
    _mesh_renderer.set_buffer_data(_p_mesh);

    _mesh_renderer.use_instancing = true;
    _mesh_renderer.instance_count = m_corner_instance_data.size();
    _mesh_renderer.set_instance_data(m_corner_instance_data);
}

void Grid3D::store_corners_refs(entt::registry& _registry)
{
    for (size_t z = 0; z < m_depth; ++z) {
        for (size_t y = 0; y < m_height; ++y) {
            for (size_t x = 0; x < m_width; ++x) {

                auto tile_entity = at(x, y, z);
                auto& tile = _registry.get<Node3D>(tile_entity);

                // Each tile has 8 corners: (x,y,z) to (x+1,y+1,z+1)
                for (size_t i = 0; i < 8; ++i)
                {
                    // Offsets for cube corners
                    int dx = (i & 1) ? 1 : 0;
                    int dy = (i & 2) ? 1 : 0;
                    int dz = (i & 4) ? 1 : 0;

                    size_t cx = x + dx;
                    size_t cy = y + dy;
                    size_t cz = z + dz;

                    size_t cidx = corner_index(cx, cy, cz);
                    tile.corner_nodes[i] = m_corner_data[cidx];

                    // std::cout << "  Corner[" << i << "]: "
                    //          << "offset(" << dx << "," << dy << "," << dz << ") "
                    //          << "-> corner(" << cx << "," << cy << "," << cz << ") "
                    //          << "index=" << cidx
                    //          << " entity=" << (int)tile.corner_nodes[i]
                    //          << "\n";
                }

                //std::cout << "\n";
            }
        }
    }
}

void Grid3D::store_tile_refs(entt::registry& _registry)
{
    for (size_t z = 0; z < m_depth + 1; ++z) {
        for (size_t y = 0; y < m_height + 1; ++y) {
            for (size_t x = 0; x < m_width + 1; ++x) {

                entt::entity corner_entity = node_at(x, y, z);
                auto& corner = _registry.get<Node3D>(corner_entity); // must be Corner3D

                // reset
                for (size_t i = 0; i < 8; ++i)
                    corner.corner_nodes[i] = entt::null;

                size_t _idx = 0;

                // Each corner may belong to up to 8 tiles
                for (int dz = -1; dz <= 0; ++dz) {
                    for (int dy = -1; dy <= 0; ++dy) {
                        for (int dx = -1; dx <= 0; ++dx) {

                            int tx = static_cast<int>(x) + dx;
                            int ty = static_cast<int>(y) + dy;
                            int tz = static_cast<int>(z) + dz;

                            std::cout << tx << " " << ty << " " << tz << "\n";

                            // skip tiles out of bounds
                            if (tx < 0 || ty < 0 || tz < 0)       continue;
                            if (tx >= static_cast<int>(m_width))  continue;
                            if (ty >= static_cast<int>(m_height)) continue;
                            if (tz >= static_cast<int>(m_depth))  continue;

                            if (_idx >= 8)
                                continue; // safety check

                            entt::entity tile_entity = at(tx, ty, tz);

                            corner.corner_nodes[_idx] = tile_entity;

                            std::cout << corner << std::endl;

                            // std::cout << "Corner (" << x << "," << y << "," << z << ") -> "
                            //         << "Tile[" << tx << " " <<  ty << " " << tz <<  " ] = " << static_cast<int>(tile_entity) << "\n";

                            ++_idx; // increment!
                        }
                    }
                }

                std::cout << "\n";
            }
        }
    }
}

bool Grid3D::is_occupied(entt::registry& _registry, size_t _x, size_t _y, size_t _z)
{
    auto& _e = at(_x, _y, _z);
    if (auto* _tile = _registry.try_get<Node3D>(_e))
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
    auto& _tile      = _registry.emplace<Node3D>   (_e, Node3D(x, y, z));
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
