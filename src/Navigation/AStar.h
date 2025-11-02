#ifndef ASTAR_H
#define ASTAR_H

#include "../PCH.h"
#include "../Component.h"
#include "../Navigation/Navigation.h"

namespace NAV
{
    std::vector<Edge> generate_edges(entt::registry& _registry, const std::vector<entt::entity>& _tracks, const std::unordered_map<NodeIndex, entt::entity>& _track_map);

    float heuristic_dist(entt::registry& _registry, Track* a, Track* b);

    std::unordered_map<Track*, std::vector<Track*>> build_track_adjacency(entt::registry& _registry, const std::vector<Edge>& edges);

    std::vector<Track*> a_star_search_tracks(entt::registry& reg,
      Track* start,
      Track* goal,
      const std::unordered_map<Track*, std::vector<Track*>>& _adjacency);

    inline TrackNode* find_exit_node(Track* from, Track* to)
    {
        for (auto& _track_node : from->track_nodes)
        {
            if (!_track_node.is_active)
                continue;
            if (_track_node.exit_dir != ConnectDirection::NONE)
                return &_track_node;
        }
        return nullptr;
    }

    inline TrackNode* find_entry_node(Track* to, Track* from)
    {
        for (auto& _track_node : to->track_nodes)
        {
            if (!_track_node.is_active)
                continue;
            if (_track_node.entry_dir != ConnectDirection::NONE)
                return &_track_node;
        }
        return nullptr;
    }

    inline std::vector<TrackNode*> expand_to_node_path(const std::vector<Track*>& track_path)
    {
        std::vector<TrackNode*> nodes;

        for (size_t i = 0; i + 1 < track_path.size(); ++i)
        {
            Track* cur  = track_path[i];
            Track* next = track_path[i + 1];

            TrackNode* exit  = find_exit_node (cur, next);
            TrackNode* entry = find_entry_node(next, cur);

            if (exit)
                nodes.push_back(exit);
            if (entry)
                nodes.push_back(entry);
        }
        return nodes;
    }

    inline std::vector<TrackNode*> translate_to_track_nodes(const std::vector<Track*>& track_path)
    {
        std::vector<TrackNode*> _track_nodes;

        if (track_path.size() < 3)
            return _track_nodes;

        for (size_t i = 1; i < track_path.size() - 1; ++i)
        {
            Track* _previous_track = track_path[i - 1];
            Track* _current_track  = track_path[i];
            Track* _next_track     = track_path[i + 1];

            NodeIndex _previous_node_index = _previous_track->node_index;
            NodeIndex _current_node_index  = _current_track->node_index;
            NodeIndex _next_node_index     = _next_track   ->node_index;

            ConnectDirection _entry_direction = ConnectDirection::NONE;

            if (_previous_node_index.idx > _current_node_index.idx)
                _entry_direction = ConnectDirection::EAST;
            else if (_previous_node_index.idx < _current_node_index.idx)
                _entry_direction = ConnectDirection::WEST;
            else if (_previous_node_index.idz > _current_node_index.idz)
                _entry_direction = ConnectDirection::SOUTH;
            else if (_previous_node_index.idz < _current_node_index.idz)
                _entry_direction = ConnectDirection::NORTH;

            ConnectDirection _exit_direction = ConnectDirection::NONE;

            if (_next_node_index.idx > _current_node_index.idx)
                _exit_direction = ConnectDirection::EAST;
            else if (_next_node_index.idx < _current_node_index.idx)
                _exit_direction = ConnectDirection::WEST;
            else if (_next_node_index.idz > _current_node_index.idz)
                _exit_direction = ConnectDirection::SOUTH;
            else if (_next_node_index.idz < _current_node_index.idz)
                _exit_direction = ConnectDirection::NORTH;

            TrackNode* _track_node = _current_track->get_track_node(_entry_direction, _exit_direction);

            if (_track_node)
                _track_nodes.push_back(_track_node);
        }
        return _track_nodes;
    }

    inline std::vector<glm::vec3> translate_to_world_position(entt::registry& _registry, const std::vector<TrackNode*>& _track_nodes)
    {
        std::vector<glm::vec3> _world_positions;

        for (TrackNode* _track_node : _track_nodes)
        {
            _world_positions.push_back(_track_node->entry_world_pos(_registry));
            _world_positions.push_back(_track_node->exit_world_pos (_registry) );
        }

        return _world_positions;
    }

    void print_edges(entt::registry& _registry, const std::vector<Edge>& _edges);


}

#endif //ASTAR_H
