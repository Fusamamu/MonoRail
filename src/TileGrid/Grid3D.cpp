#include "Grid3D.h"
#include "ApplicationConfig.h"

#include "Asset/AssetManager.h"
#include "Navigation/Navigation.h"

#include "Renderer/Material.h"
#include "Renderer/MeshRenderer.h"

#include "Components/Component.h"
#include "Components/Camera.h"
#include "Ray.h"

#include "TileTable.h"
#include "TileGrid/Utility.h"
#include "Procedural/PerlinNoise.h"

namespace TileGrid
{
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

    void Grid3D::update(entt::registry &_registry, Component::Camera _camera, InputSystem& _input_system)
    {
        switch (mode)
        {
            case EditMode::NONE:
                break;
            case EditMode::ADD_TILE:
                break;
            case EditMode::REMOVE_TILE:
                break;
            case EditMode::MARK_TILE:
                if (_input_system.left_mouse_pressed())
                {
                    Ray _ray = _camera.screen_point_to_ray(_input_system.get_mouse_pos(), g_app_config.screen_size());
                    float _dist = 0.0f;
                    entt::entity _entity = ray_cast_select_entity(_registry, _ray, _dist);
                    if (_entity != entt::null)
                    {
                        if (start_e != entt::null)
                        {
                            auto& _material = _registry.get<Material>(start_e);
                            _material.diffuse_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                        }
                        start_e = _entity;
                        if (Material* _mat = _registry.try_get<Material>(_entity))
                        {
                            _mat->diffuse_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
                        }
                    }
                }

            if (_input_system.right_mouse_pressed())
            {
                Ray _ray = _camera.screen_point_to_ray(_input_system.get_mouse_pos(), g_app_config.screen_size());
                float _dist = 0.0f;
                entt::entity _entity = ray_cast_select_entity(_registry, _ray, _dist);
                if (_entity != entt::null)
                {
                    if (dest_e != entt::null)
                    {
                        auto& _material = _registry.get<Material>(dest_e);
                        _material.diffuse_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                    }
                    dest_e = _entity;
                    if (Material* _mat = _registry.try_get<Material>(_entity))
                    {
                        _mat->diffuse_color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
                    }
                }
            }

            if (_input_system.is_key_down(SDL_SCANCODE_SPACE))
            {
                std::optional<std::vector<entt::entity>> _path = find_path(_registry);
                if (_path.has_value())
                {
                    auto _path_v = _path.value();
                    for (entt::entity _entity : _path_v)
                    {
                        if (Node3D* _tile = _registry.try_get<Node3D>(_entity))
                        {
                        }
                    }
                }
            }
            break;
            default: ;
        }
    }

    const entt::entity& Grid3D::entity_at(NodeIndex _node_index) const
    {
        if (out_of_bounds(_node_index))
            return entt::null;
        return m_data[tile_index(_node_index)];
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

    uint8_t Grid3D::get_surrounding_bit(entt::registry& _registry, NodeIndex _node_index)
    {
        uint8_t _bitmask = 0b00000000;
        for (size_t i = 0; i < VICINITY_8_DIR.size(); ++i)
        {
            NodeIndex neighbor = _node_index + VICINITY_8_DIR[i];
            if (out_of_bounds(neighbor))
                continue;
            if (is_occupied(_registry, neighbor))
                _bitmask |= (1 << i);
        }
        return _bitmask;
    }

    uint8_t Grid3D::get_surrounding_bitmask_4direction(entt::registry& _registry, NodeIndex _node_index)
    {
        uint8_t _bitmask = 0;
        for (size_t i = 0; i < VICINITY_4_DIR.size(); ++i)
        {
            NodeIndex neighbor = _node_index + VICINITY_4_DIR[i];
            if (out_of_bounds(neighbor))
                continue;
            if (is_occupied(_registry, neighbor))
                _bitmask |= (1 << i);
        }
        _bitmask &= 0b00001111; // Ensure only lower 4 bits are used
        return _bitmask;
    }

    entt::entity& Grid3D::corner_node_at(size_t x, size_t y, size_t z)
    {
        //check_bounds(x, y, z);
        return m_corner_data[corner_index(x, y, z)];
    }

    const entt::entity& Grid3D::corner_node_at(size_t x, size_t y, size_t z) const
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

                    m_tile_instance_data.push_back(_instance_data);

                    at(x, y, z) = _e;
                }
            }
        }
    }

    void Grid3D::fill_tile_at_level(entt::registry& _registry, uint32_t _level)
    {
        for (size_t z = 0; z < m_depth; ++z) {
            for (size_t x = 0; x < m_width; ++x) {
                fill_tile_at(_registry, NodeIndex(x, _level, z));
            }
        }
    }

    void Grid3D::create_tile_instance(entt::registry& _registry)
    {
        auto _e = _registry.create();

        m_tile_mesh_e = _e;

        MUG::Mesh* _p_mesh = AssetManager::instance().get_first_mesh("bevel_cube");

        auto& _node = _registry.emplace<Node>(_e, Node());
        _node.name = "cube grid";

        auto& _transform = _registry.emplace<Component::Transform>(_e);
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
        _material_comp.cast_shadow = false;

        auto& _mesh_renderer = _registry.emplace<MeshRenderer>(_e);
        _mesh_renderer.load_mesh      (_p_mesh);
        _mesh_renderer.set_buffer_data(_p_mesh);

        _mesh_renderer.use_instancing = true;
        _mesh_renderer.instance_count = m_tile_instance_data.size();
        _mesh_renderer.set_instance_data(m_tile_instance_data);
    }

    void Grid3D::generate_tiles_with_perlin(entt::registry& _registry) {
        Procgen::PerlinNoise noise(12345); // fixed seed

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

                    m_corner_instance_data.push_back(_instance_data);

                    corner_node_at(x, y, z) = _e;
                }
            }
        }
    }

    void Grid3D::create_corner_instance(entt::registry& _registry)
    {
        entt::entity _e = _registry.create();

        MUG::Mesh* _p_mesh = AssetManager::instance().get_first_mesh("sphere");

        auto& _node = _registry.emplace<Node>(_e, Node());
        _node.name = "corner_grid";

        auto& _transform = _registry.emplace<Component::Transform>(_e);
        _transform.position = glm::vec3(0.0f);

        Material _material;
        _material.shader_id     = "object_instance";
        _material.rgba          = glm::vec4(1.0f, 0.0f, 0.0f, 0.1f);

        auto& _material_comp = _registry.emplace<Material>(_e);
        _material_comp.shader_id   = _material.shader_id;
        _material_comp.depth_test  = _material.depth_test;
        _material_comp.depth_write = _material.depth_write;
        _material_comp.diffuseMap  = _material.diffuseMap;
        _material_comp.rgba        = _material.rgba;
        _material_comp.cast_shadow = false;

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

                        //std::cout << "corner index: " << cx << " " << cz << " " << cy << std::endl;
                    }
                }
            }
        }
    }

    void Grid3D::store_tile_refs(entt::registry& _registry)
    {
        for (size_t z = 0; z < m_depth + 1; ++z) {
            for (size_t y = 0; y < m_height + 1; ++y) {
                for (size_t x = 0; x < m_width + 1; ++x) {

                    entt::entity corner_entity = corner_node_at(x, y, z);
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

                                    if (out_of_bounds(_node_index))
                                    {
                                        ++_idx;
                                        continue;
                                    }

                                    entt::entity tile_entity = at(tx, ty, tz);
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

                                    if (out_of_bounds(_node_index))
                                    {
                                        ++_idx;
                                        continue;
                                    }

                                    entt::entity tile_entity = at(tx, ty, tz);
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

    void Grid3D::update_corner_nodes(entt::registry& _registry)
    {
        for (size_t z = 0; z < m_depth + 1; ++z) {
            for (size_t y = 0; y < m_height + 1; ++y) {
                for (size_t x = 0; x < m_width + 1; ++x) {

                    entt::entity _corner_entity = corner_node_at(x, y, z);
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

    bool Grid3D::is_occupied(entt::registry& _registry, size_t _x, size_t _y, size_t _z)
    {
        auto& _e = at(_x, _y, _z);
        if (auto* _tile = _registry.try_get<Node3D>(_e))
            return _tile->is_occupied;
        return false;
    }

    bool Grid3D::is_occupied(entt::registry& _registry, NodeIndex _node_index)
    {
        auto& _e = at(_node_index.idx, _node_index.idy, _node_index.idz);
        if (auto* _tile = _registry.try_get<Node3D>(_e))
            return _tile->is_occupied;
        return false;
    }

    NAV::Track* Grid3D::add_track(entt::registry& _registry, NodeIndex _at_node_index, glm::vec3 _position)
    {
        if (out_of_bounds(_at_node_index))
            return nullptr;
        if (is_occupied(_registry, _at_node_index.idx, _at_node_index.idy, _at_node_index.idz))
            return nullptr;

        const entt::entity& _e = entity_at(_at_node_index);

        auto& _node      = _registry.get<Node>     (_e);
        auto& _node3D    = _registry.get<Node3D>   (_e);
        auto& _transform = _registry.get<Component::Transform>(_e);

        _node  .is_active   = true;
        _node3D.is_occupied = true;
        _node3D.type        = TileType::NONE;

        _transform.position = _position;
        _transform.scale    = glm::vec3(1.0f);

        auto& _aabb          = _registry.emplace<AABB>        (_e);
        auto& _material      = _registry.emplace<Material>    (_e);
        auto& _mesh_renderer = _registry.emplace<MeshRenderer>(_e);
        auto& _track         = _registry.emplace<NAV::Track>  (_e);

        _aabb.min = { -0.5f, -0.1f, -0.5f };
        _aabb.max = {  0.5f,  0.1f,  0.5f };

        _material.shader_id = "phong";

        {
            uint8_t _bitmask  = get_surrounding_bitmask_4direction(_registry, _at_node_index);
            std::string _mesh = TILE_TABLE::get_tile_name(_bitmask);
            MUG::Mesh* _tile_mesh  = AssetManager::instance().get_first_mesh(_mesh);

            _mesh_renderer.load_mesh      (_tile_mesh);
            _mesh_renderer.set_buffer_data(_tile_mesh);

            _track.node_index     = _at_node_index;
            _track.world_position = _transform.position;
            _track.self_entity    = _e;
            _track.init(_bitmask);
        }

        for (size_t i = 0; i < VICINITY_8_DIR.size(); ++i)
        {
            NodeIndex _at_n_node_index = _at_node_index + VICINITY_8_DIR[i];
            if (out_of_bounds(_at_n_node_index))
                continue;

            if (auto [_track, _mesh_renderer] = _registry.try_get<NAV::Track, MeshRenderer>(entity_at(_at_n_node_index)); _track && _mesh_renderer)
            {
                {
                    uint8_t _bitmask  = get_surrounding_bitmask_4direction(_registry, _at_n_node_index);
                    std::string _mesh = TILE_TABLE::get_tile_name(_bitmask);
                    MUG::Mesh* _tile_mesh  = AssetManager::instance().get_first_mesh(_mesh);

                    _mesh_renderer->load_mesh      (_tile_mesh);
                    _mesh_renderer->set_buffer_data(_tile_mesh);

                    _track->init(_bitmask);
                }
            }
        }

        add_tile_animation(_registry, _e);

        return &_track;
    }

    void Grid3D::add_tile_above(entt::registry& _registry,
        const std::string& _mesh,
        size_t x, size_t y, size_t z,
        glm::vec3 _position,
        TileType _tile_type)
    {
        add_tile_at(_registry, _mesh, x, y + 1, z, _position, _tile_type);
    }

    void Grid3D::add_tile_at(entt::registry& _registry,
        const std::string& _mesh,
        size_t x, size_t y, size_t z,
        glm::vec3 _position,
        TileType _tile_type)
    {
        if (out_of_bounds(x, y, z))
            return;
        if (is_occupied(_registry, x, y, z))
        {
            std::cout << "Cannot add tile at " << x << ", " << y << ", " << z << std::endl;
            return;
        }

        entt::entity& _e = at(x, y, z);

        auto& _node      = _registry.get<Node>     (_e);
        auto& _node3D    = _registry.get<Node3D>   (_e);
        auto& _transform = _registry.get<Component::Transform>(_e);

        _node  .is_active   = true;
        _node3D.is_occupied = true;
        _node3D.type        = _tile_type;

        _transform.position = _position;
        _transform.scale    = glm::vec3(1.0f);

        auto& _aabb          = _registry.emplace<AABB>        (_e);
        auto& _material      = _registry.emplace<Material>    (_e);
        auto& _mesh_renderer = _registry.emplace<MeshRenderer>(_e);

        _aabb.min = { -0.5f, -0.5f, -0.5f };
        _aabb.max = {  0.5f,  0.5f,  0.5f };

        _material.shader_id = "phong";

        MUG::Mesh* _tile_mesh = AssetManager::instance().get_first_mesh(_mesh);

        _mesh_renderer.load_mesh      (_tile_mesh);
        _mesh_renderer.set_buffer_data(_tile_mesh);

        add_tile_animation(_registry, _e);
    }

    void Grid3D::fill_tile_at(entt::registry& _registry, NodeIndex _at_node_index)
    {
        if (out_of_bounds(_at_node_index))
            return;
        if (is_occupied(_registry, _at_node_index))
            return;

        const entt::entity& _e = entity_at(_at_node_index);

        auto& _node      = _registry.get<Node>     (_e);
        auto& _node3D    = _registry.get<Node3D>   (_e);
        auto& _aabb      = _registry.emplace<AABB> (_e);

        _node  .is_active   = true;
        _node3D.is_occupied = true;
        _node3D.type = TileType::GROUND;

        _aabb.min = { -0.5f, -0.5f, -0.5f };
        _aabb.max = {  0.5f,  0.5f,  0.5f };
    }

    void Grid3D::add_tile_at(entt::registry& _registry, const TileData& _tile_data)
    {
        if (out_of_bounds(_tile_data.node_index))
            return;
        if (is_occupied(_registry, _tile_data.node_index))
            return;

        entt::entity& _e = at(_tile_data.node_index.idx, _tile_data.node_index.idy, _tile_data.node_index.idz);

        auto& _node      = _registry.get<Node>     (_e);
        auto& _node3D    = _registry.get<Node3D>   (_e);
        auto& _transform = _registry.get<Component::Transform>(_e);

        _node  .is_active   = true;
        _node3D.is_occupied = true;

        _transform.position = _tile_data.position;
        _transform.scale    = glm::vec3(1.0f);

        auto& _aabb          = _registry.emplace<AABB> (_e);
        auto& _material      = _registry.emplace<Material>(_e);
        auto& _mesh_renderer = _registry.emplace<MeshRenderer>(_e);

        _aabb.min = { -0.5f, -0.5f, -0.5f };
        _aabb.max = {  0.5f,  0.5f,  0.5f };

        _material.shader_id = "phong";

        MUG::Mesh* _tile_mesh = AssetManager::instance().get_first_mesh(_tile_data.mesh_name);

        _mesh_renderer.load_mesh      (_tile_mesh);
        _mesh_renderer.set_buffer_data(_tile_mesh);

        add_tile_animation(_registry, _e);
    }

    bool Grid3D::out_of_bounds(size_t x, size_t y, size_t z) const
    {
        if (x >= m_width || y >= m_height || z >= m_depth)
            return true;
        return false;
    }

    bool Grid3D::out_of_bounds(NodeIndex _node_index) const
    {
        if (_node_index.idx >= m_width  || _node_index.idx < 0 ||
            _node_index.idy >= m_height || _node_index.idy < 0 ||
            _node_index.idz >= m_depth  || _node_index.idz < 0)
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

    std::optional<std::vector<entt::entity>> Grid3D::find_path(entt::registry& _registry, NodeIndex _start_node, NodeIndex _dest_node)
    {
        if (out_of_bounds(_start_node) || out_of_bounds(_dest_node))
            return std::nullopt;

        const size_t _total     = m_width * m_height * m_depth;
        const size_t _start_idx = tile_index(_start_node);
        const size_t _goal_idx  = tile_index(_dest_node );

        if (_start_idx == _goal_idx)
        {
            entt::entity e = m_data[_start_idx];
            if (e == entt::null)
                return std::nullopt;
            return std::vector<entt::entity>{ e };
        }

        const float INF = std::numeric_limits<float>::infinity();

        std::vector<float>  g_score(_total, INF);          // g_score = cost from start to node
        std::vector<float>  f_score(_total, INF);          // f_score = g + heuristic
        std::vector<size_t> came_from(_total, SIZE_MAX);   // came_from: previous index (SIZE_MAX means none)

        // min-heap ordered by f_score; pair<f_score, index>
        using PQItem = std::pair<float, size_t>;
        struct Compare {
            bool operator()(const PQItem& a, const PQItem& b) const
            {
                return a.first > b.first;
            }
        };
        std::priority_queue<PQItem, std::vector<PQItem>, Compare> open_set;

        // initialize
        g_score[_start_idx] = 0.0f;
        f_score[_start_idx] = heuristic_manhattan(_start_node, _dest_node);

        open_set.push({ f_score[_start_idx], _start_idx });

        const std::array<NodeIndex, 6> _vicinity_dir =
            {
            NodeIndex( 1, 0, 0),
            NodeIndex(-1, 0, 0),
            NodeIndex( 0, 1, 0),
            NodeIndex( 0,-1, 0),
            NodeIndex( 0, 0, 1),
            NodeIndex( 0, 0,-1),
        };

        std::vector<char> closed(_total, 0);

        while (!open_set.empty())
        {
            auto [current_f, current_idx] = open_set.top();
            open_set.pop();

            if (closed[current_idx])
                continue; // already processed (stale PQ entries)
            closed[current_idx] = 1;

            if (current_idx == _goal_idx)
            {
                std::vector<entt::entity> _path; // reconstruct path
                size_t cur = _goal_idx;
                while (cur != SIZE_MAX)
                {
                    entt::entity e = m_data[cur];
                    if (e == entt::null)
                        return std::nullopt; // if any tile in path is null, consider path invalid
                    _path.push_back(e);
                    cur = came_from[cur];
                }
                std::reverse(_path.begin(), _path.end());
                return _path;
            }

            NodeIndex _current_node = tile_index_to_node_coord(current_idx);

            for (NodeIndex _vicinity : _vicinity_dir)
            {
                NodeIndex _neighbor = _current_node + _vicinity;

                if (out_of_bounds(_neighbor))
                    continue;

                size_t neighbor_idx = tile_index(_neighbor);

                if (neighbor_idx != _goal_idx && is_occupied(_registry, _neighbor))
                    continue;

                float tentative_g = g_score[current_idx] + 1.0f;

                if (tentative_g < g_score[neighbor_idx])
                {
                    came_from[neighbor_idx] = current_idx;

                    g_score[neighbor_idx] = tentative_g;
                    f_score[neighbor_idx] = tentative_g + heuristic_manhattan(_neighbor, _dest_node);

                    open_set.push({ f_score[neighbor_idx], neighbor_idx });
                }
            }
        }

        return std::nullopt;
    }

    std::optional<std::vector<entt::entity>> Grid3D::find_path(entt::registry& _registry)
    {
        if (start_e == entt::null || dest_e == entt::null)
            return std::nullopt;

        Node3D* _start = _registry.try_get<Node3D>(start_e);
        Node3D* _dest  = _registry.try_get<Node3D>(dest_e);
        if (!_start || !_dest)
            return std::nullopt;

        return find_path(_registry, _start->to_node_index(), _dest->to_node_index());
    }

    std::vector<uint8_t> Grid3D::get_voxel_data(entt::registry& _registry)
    {
        std::vector<uint8_t>  _data;
        _data.resize(m_data.size(), 0);

        for(size_t _z = 0; _z < m_depth; ++_z){
            for(size_t _y = 0; _y < m_height; ++_y){
                for(size_t _x = 0; _x < m_width; ++_x){

                    Node3D _node = _registry.get<Node3D>(at(_x, _y, _z));
                    if (_node.is_occupied)
                        _data[_z * (m_width * m_height) + _y * m_width + _x] = 255;
                }
            }
        }


        std::cout << std::endl;
        std::cout << std::endl;

        for(size_t _z = 0; _z < m_depth; ++_z){
            for(size_t _x = 0; _x < m_width; ++_x){


                uint8_t _v = _data[_z * (m_width * m_height) + 1 * m_width + _x];

                std::cout << _v << ", ";
            }
            std::cout << std::endl;
        }

        return _data;
    }
}















