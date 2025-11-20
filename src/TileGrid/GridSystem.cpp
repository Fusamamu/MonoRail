#include "GridSystem.h"
#include "Asset/AssetManager.h"
#include "Procedural/PerlinNoise.h"
#include "Renderer/Material.h"

namespace TileGrid
{
    void GridSystem::generate_tiles(entt::registry& _registry, Grid3D& _grid)
    {
        //m_tile_instance_data.resize(m_data.size());

        for (size_t z = 0; z < _grid.get_depth(); ++z) {
            for (size_t y = 0; y < _grid.get_height(); ++y) {
                for (size_t x = 0; x < _grid.get_width(); ++x) {

                    entt::entity _e = _registry.create();

                    auto& _node         = _registry.emplace<Node>     (_e);
                    auto& _node3D       = _registry.emplace<Node3D>   (_e, Node3D(x, y, z));
                    auto& _transform    = _registry.emplace<Component::Transform>(_e);

                    _node.name          = "tile";
                    _node.is_active     = true;
                    _node.is_static     = true;
                    _node3D.is_occupied = false;

                    _transform.position  = glm::vec3(x, y, z);
                    _transform.scale     = glm::vec3(0.2f);
                    _transform.world_mat = _transform.get_local_mat4();

                    InstanceData _instance_data;
                    _instance_data.model = _transform.get_local_mat4();
                    _instance_data.color = glm::vec4(0.0f, 1.0f, 0.0f, 0.05f);

                    //m_tile_instance_data.push_back(_instance_data);

                    _grid.at(x, y, z) = _e;
                }
            }
        }
    }

    void GridSystem::generate_tiles_with_perlin(entt::registry& _registry, Grid3D& _grid)
    {
        Procgen::PerlinNoise noise(12345); // fixed seed

        for (size_t z = 0; z < _grid.get_depth(); ++z) {
            for (size_t y = 0; y < _grid.get_height(); ++y) {
                for (size_t x = 0; x < _grid.get_width(); ++x) {

                    double nx = static_cast<double>(x) / _grid.get_width();
                    double ny = static_cast<double>(y) / _grid.get_height();
                    double nz = static_cast<double>(z) / _grid.get_depth();

                    double value = 0.0;

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
                    MUG::Mesh* _tile_mesh = AssetManager::instance().get_first_mesh(mesh_name);

                    auto& _tile = _registry.emplace<Node3D>(_e, Node3D(x, y, z));

                    auto& _node = _registry.emplace<Node>(_e);
                    _node.name = tile_type;

                    auto& _transform = _registry.emplace<Component::Transform>(_e);
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

                    _grid.at(x, y, z) = _e;
                }
            }
        }
    }

    void GridSystem::generate_corner_nodes(entt::registry& _registry, Grid3D& _grid)
    {
        //m_corner_instance_data.reserve(m_corner_data.size());

        for (size_t z = 0; z < _grid.get_depth() + 1; ++z) {
            for (size_t y = 0; y < _grid.get_height() + 1; ++y) {
                for (size_t x = 0; x < _grid.get_width() + 1; ++x) {

                    entt::entity _e = _registry.create();

                    auto& _node      = _registry.emplace<Node>     (_e);
                    auto& _node3D    = _registry.emplace<Node3D>   (_e, Node3D(x, y, z));
                    auto& _transform = _registry.emplace<Component::Transform>(_e);

                    _node.name          = "corner_node";
                    _node.is_active     = true;
                    _node.is_static     = true;
                    _node3D.is_occupied = true;

                    _transform.position = glm::vec3(x, y, z) - glm::vec3(0.5f);
                    _transform.scale    = glm::vec3(0.1f, 0.1f, 0.1f);

                    _transform.world_mat = _transform.get_local_mat4();

                    InstanceData _instance_data;
                    _instance_data.model = _transform.get_local_mat4();
                    _instance_data.color = glm::vec4(0.1f, 1.0f, 1.0f, 0.1f);

                    //m_corner_instance_data.push_back(_instance_data);

                    _grid.corner_node_at(x, y, z) = _e;
                }
            }
        }
    }

    void GridSystem::create_tile_instance(entt::registry& _registry, Grid3D& _grid)
    {
    }

    void GridSystem::create_corner_instance(entt::registry& _registry, Grid3D& _grid)
    {
    }

    void GridSystem::store_corners_refs(entt::registry& _registry, Grid3D& _grid)
    {
        for (size_t z = 0; z < _grid.get_depth(); ++z) {
            for (size_t y = 0; y < _grid.get_height(); ++y) {
                for (size_t x = 0; x < _grid.get_width(); ++x) {

                    auto tile_entity = _grid.at(x, y, z);
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

                        size_t cidx = _grid.corner_index(cx, cy, cz);
                        tile.corner_nodes[i] = _grid.m_corner_data[cidx];

                        //std::cout << "corner index: " << cx << " " << cz << " " << cy << std::endl;
                    }
                }
            }
        }
    }

    void GridSystem::store_tile_refs(entt::registry& _registry, Grid3D& _grid)
    {
          for (size_t z = 0; z < _grid.get_depth() + 1; ++z) {
            for (size_t y = 0; y < _grid.get_height() + 1; ++y) {
                for (size_t x = 0; x < _grid.get_width() + 1; ++x) {

                    entt::entity corner_entity = _grid.corner_node_at(x, y, z);
                    auto& corner = _registry.get<Node3D>(corner_entity); // must be Corner3D

                    for (size_t i = 0; i < 8; ++i)
                        corner.corner_nodes[i] = entt::null;

                    size_t _idx = 0;

                    for (int dy = -1; dy <= 0; ++dy){
                        for (int dz = -1; dz <= 0; ++dz){

                            int tx, ty, tz;

                            if (dz == -1){
                                for (int dx = 0; dx > -2; --dx){
                                    tx = static_cast<int>(x) + dx;
                                    ty = static_cast<int>(y) + dy;
                                    tz = static_cast<int>(z) + dz;

                                    NodeIndex _node_index (tx, ty, tz);

                                    if (_grid.out_of_bounds(_node_index))
                                    {
                                        ++_idx;
                                        continue;
                                    }

                                    entt::entity tile_entity = _grid.at(tx, ty, tz);
                                    corner.corner_nodes[_idx] = tile_entity;
                                    ++_idx;
                                }
                            }
                            else{
                                for (int dx = -1; dx <= 0; ++dx){
                                    tx = static_cast<int>(x) + dx;
                                    ty = static_cast<int>(y) + dy;
                                    tz = static_cast<int>(z) + dz;

                                    NodeIndex _node_index (tx, ty, tz);

                                    if (_grid.out_of_bounds(_node_index))
                                    {
                                        ++_idx;
                                        continue;
                                    }

                                    entt::entity tile_entity = _grid.at(tx, ty, tz);
                                    corner.corner_nodes[_idx] = tile_entity;
                                    ++_idx;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void GridSystem::update_corner_nodes(entt::registry& _registry, Grid3D& _grid)
    {
        for (size_t z = 0; z < _grid.get_depth() + 1; ++z) {
            for (size_t y = 0; y < _grid.get_height() + 1; ++y) {
                for (size_t x = 0; x < _grid.get_width() + 1; ++x) {

                    entt::entity _corner_entity = _grid.corner_node_at(x, y, z);
                    auto& _corner = _registry.get<Node3D>(_corner_entity); // must be Corner3D

                    Component::Transform   & _transform     = _registry.get_or_emplace<Component::Transform>   (_corner_entity);
                    MeshRenderer& _mesh_renderer = _registry.get_or_emplace<MeshRenderer>(_corner_entity);
                    Material    & _material      = _registry.get_or_emplace<Material>    (_corner_entity);

                    _transform.scale     = glm::vec3(1.0f);
                    _transform.world_mat = _transform.get_local_mat4();
                    _material.shader_id  = "tile";

                    uint8_t _bitmask       = _corner.to_bitmask(_registry);
                    std::string _mesh_name = to_formatted_name(_bitmask);

                    MUG::Mesh* _tile_mesh = AssetManager::instance().get_first_mesh(_mesh_name);
                    if (_tile_mesh)
                    {
                        _mesh_renderer.load_mesh      (_tile_mesh);
                        _mesh_renderer.set_buffer_data(_tile_mesh);
                    }
                    else
                    {
                        MUG::Mesh* _fallback = AssetManager::instance().get_first_mesh("sphere");
                        _transform.scale     = glm::vec3(0.0f);
                        _transform.world_mat = _transform.get_local_mat4();
                        _mesh_renderer.load_mesh      (_fallback);
                        _mesh_renderer.set_buffer_data(_fallback);
                    }
                }
            }
        }
    }

    void GridSystem::fill_tile_at_level(entt::registry& _registry, uint32_t _level)
    {
    }

    void GridSystem::fill_tile_at(entt::registry& _registry, NodeIndex _at_node_index)
    {
    }

    NAV::Track* GridSystem::add_track(entt::registry& _registry, NodeIndex _at_node_index, glm::vec3 _position)
    {
        return nullptr;
    }

    void GridSystem::add_tile_at(entt::registry& _registry, const std::string& _mesh, size_t x, size_t y, size_t z, glm::vec3 _position, TileType _tile_type)
    {
    }

    void GridSystem::add_tile_above(entt::registry& _registry, const std::string& _mesh, size_t x, size_t y, size_t z, glm::vec3 _position, TileType _tile_type)
    {
    }

    void GridSystem::select_tile_at(entt::registry& _registry, size_t x, size_t y, size_t z)
    {
    }

    void GridSystem::update(entt::registry& _registry, Component::Camera _camera, InputSystem& _input_system)
    {
    }

    std::optional<std::vector<entt::entity>> GridSystem::find_path(entt::registry& _registry, NodeIndex _start_node, NodeIndex _dest_node)
    {
        return std::nullopt;
    }

    std::optional<std::vector<entt::entity>> GridSystem::find_path(entt::registry& _registry)
    {
        return std::nullopt;
    }
}
