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


    void print_edges(entt::registry& _registry, const std::vector<Edge>& _edges);


}

#endif //ASTAR_H
