#ifndef GRID_SYSTEM_H
#define GRID_SYSTEM_H

#include "Grid3D.h"

namespace TileGrid
{
    class GridSystem
    {
    public:
        void generate_tiles            (entt::registry& _registry, Grid3D& _grid);
        void generate_tiles_with_perlin(entt::registry& _registry, Grid3D& _grid);
        void generate_corner_nodes     (entt::registry& _registry, Grid3D& _grid);
        void create_tile_instance      (entt::registry& _registry, Grid3D& _grid);
        void create_corner_instance    (entt::registry& _registry, Grid3D& _grid);
        void store_corners_refs        (entt::registry& _registry, Grid3D& _grid);
        void store_tile_refs           (entt::registry& _registry, Grid3D& _grid);
        void update_corner_nodes       (entt::registry& _registry, Grid3D& _grid);

        void fill_tile_at_level        (entt::registry& _registry, uint32_t  _level        );
        void fill_tile_at              (entt::registry& _registry, NodeIndex _at_node_index);

        NAV::Track* add_track(entt::registry& _registry, NodeIndex _at_node_index, glm::vec3 _position);

        void add_tile_at   (entt::registry& _registry, const std::string& _mesh, size_t x, size_t y, size_t z, glm::vec3 _position, TileType _tile_type);
        void add_tile_above(entt::registry& _registry, const std::string& _mesh, size_t x, size_t y, size_t z, glm::vec3 _position, TileType _tile_type);

        void select_tile_at(entt::registry& _registry, size_t x, size_t y, size_t z);

        void update(entt::registry &_registry, Component::Camera _camera, InputSystem& _input_system);

        std::optional<std::vector<entt::entity>> find_path(entt::registry& _registry, NodeIndex _start_node, NodeIndex _dest_node);
        std::optional<std::vector<entt::entity>> find_path(entt::registry& _registry);
    };
}

#endif
