#include "AStar.h"

namespace NAV
{
    std::vector<Edge> generate_edges(entt::registry& _registry, const std::vector<entt::entity>& _tracks, const std::unordered_map<NodeIndex, entt::entity>& _track_map)
    {
        std::vector<Edge> edges;

        for (const entt::entity& _e : _tracks)
        {
            Track& _track = _registry.get<Track>(_e);

            for (auto& _track_node : _track.track_nodes)
            {
                if (!_track_node.is_active)
                    continue;

                NodeIndex _neighbor_index = _track.node_index;

                switch (_track_node.exit_dir)
                {
                    case ConnectDirection::NONE :                        break;
                    case ConnectDirection::NORTH: _neighbor_index.idz--; break;
                    case ConnectDirection::WEST : _neighbor_index.idx--; break;
                    case ConnectDirection::EAST : _neighbor_index.idx++; break;
                    case ConnectDirection::SOUTH: _neighbor_index.idz++; break;
                }

                auto _it = _track_map.find(_neighbor_index);
                if(_it != _track_map.end())
                {
                    Track& neighbor = _registry.get<Track>(_it->second);

                    ConnectDirection _opposite = opposite_dir(_track_node.exit_dir);

                    for(auto& _neighbor_track_node : neighbor.track_nodes)
                    {
                        if(_neighbor_track_node.is_active && _neighbor_track_node.entry_dir == _opposite)
                            edges.push_back(Edge(&_track_node, &_neighbor_track_node));
                    }
                }
            }
        }

        return edges;
    }

    float heuristic_dist(entt::registry& _registry, Track* a, Track* b)
    {
        return glm::distance(a->world_position, b->world_position);
    }

    std::unordered_map<Track*, std::vector<Track*>> build_track_adjacency(entt::registry& _registry, const std::vector<Edge>& edges)
    {
        std::unordered_map<Track*, std::vector<Track*>> adj;

        adj.reserve(edges.size() * 2);

        for (const auto& e : edges)
        {
            if (!e.from || !e.to)
                continue;

            Track* t1 = &_registry.get<Track>(e.from->track_entity);
            Track* t2 = &_registry.get<Track>(e.to  ->track_entity);

            if (!t1 || !t2)
                continue;

            adj[t1].push_back(t2);
            adj[t2].push_back(t1); // bidirectional
        }

        return adj;
    }

    std::vector<Track*> a_star_search_tracks(
        entt::registry& reg,
        Track* start,
        Track* goal,
        const std::unordered_map<Track*, std::vector<Track*>>& _adjacency)
    {
        if (!start || !goal)
            return {};

        using PQItem = std::pair<float, Track*>;
        struct Compare { bool operator()(const PQItem& a, const PQItem& b) const { return a.first > b.first; } };
        std::priority_queue<PQItem, std::vector<PQItem>, Compare> open_queue;

        std::unordered_set<Track*>         open_set, closed_set;
        std::unordered_map<Track*, Track*> came_from;
        std::unordered_map<Track*, float>  g_score, f_score;

        g_score[start] = 0.0f;
        f_score[start] = heuristic_dist(reg, start, goal);

        open_queue.push({f_score[start], start});
        open_set.insert(start);

        while (!open_queue.empty())
        {
            Track* _current = open_queue.top().second;
            open_queue.pop();
            open_set.erase(_current);

            if (_current == goal)
            {
                std::vector<Track*> path;
                Track* cur = goal;
                while (cur)
                {
                    path.push_back(cur);
                    auto it = came_from.find(cur);
                    if (it == came_from.end())
                        break;
                    cur = it->second;
                }
                std::reverse(path.begin(), path.end());

                std::cout << "Found path" << std::endl;
                return path;
            }

            closed_set.insert(_current);

            auto it = _adjacency.find(_current);
            if (it == _adjacency.end())
                continue;

            for (Track* neighbor : it->second)
            {
                if (closed_set.count(neighbor))
                    continue;

                float tentative_g = g_score[_current] + glm::distance(_current->world_position, neighbor->world_position);

                if (!g_score.count(neighbor) || tentative_g < g_score[neighbor])
                {
                    came_from[neighbor] = _current;
                    g_score[neighbor] = tentative_g;
                    f_score[neighbor] = tentative_g + heuristic_dist(reg, neighbor, goal);

                    if (!open_set.count(neighbor))
                    {
                        open_queue.push({f_score[neighbor], neighbor});
                        open_set.insert(neighbor);
                    }
                }
            }
        }

        std::cout << "Fail path" << std::endl;
        return {};
    }

    void print_edges(entt::registry& _registry, const std::vector<Edge>& _edges)
    {
        std::cout << std::endl;
        std::cout << "=== TrackGraph Edges ===\n";

        std::cout << "Edge size : " << _edges.size() << std::endl;

        for (const Edge& edge : _edges)
        {
            if (!edge.from || !edge.to)
                continue;

            // Optionally, get entity IDs or track directions
            entt::entity fromEntity = edge.from->track_entity;
            entt::entity toEntity   = edge.to  ->track_entity;

            if (Track* _from_t = _registry.try_get<Track>(fromEntity))
                if (Track* _to_t = _registry.try_get<Track>(toEntity))
                    std::cout << "Edge: [from] " << _from_t->node_index << " [to] " << _to_t->node_index << "\n";
        }

        std::cout << std::endl;
    }
}