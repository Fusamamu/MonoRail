#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "../PCH.h"
#include "../Component.h"

namespace NAV
{
    enum class ConnectDirection : uint8_t
    {
        NONE  =  99,
        NORTH =  0,
        WEST  =  1,
        EAST  =  2,
        SOUTH =  3,
    };

    inline std::string dir_to_string(ConnectDirection dir)
    {
        switch (dir)
        {
            case ConnectDirection::NORTH: return "NORTH";
            case ConnectDirection::WEST:  return "WEST";
            case ConnectDirection::EAST:  return "EAST";
            case ConnectDirection::SOUTH: return "SOUTH";
            case ConnectDirection::NONE:  return "NONE";
            default:                      return "UNKNOWN";
        }
    }

    inline glm::vec3 dir_to_local_pos(ConnectDirection _direction)
    {
        switch (_direction)
        {
            case ConnectDirection::NONE:
                return glm::vec3(0, 0, 0);
            case ConnectDirection::NORTH:
                return glm::vec3(0, 0, -0.5f);
            case ConnectDirection::WEST:
                return glm::vec3(-0.5f, 0, 0);
            case ConnectDirection::EAST:
                return glm::vec3(0.5f, 0, 0);
            case ConnectDirection::SOUTH:
                return glm::vec3(0, 0, 0.5f);
            default:
                return glm::vec3(0, 0, 0);
        }
    }

    ConnectDirection opposite_dir(ConnectDirection dir);

    constexpr std::array<uint8_t, 4> OPPOSITE_DIR =
    {
        3, 2, 1, 0
    };

    const std::array<NodeIndex, 4> VICINITY_4_DIR =
    {
        NodeIndex( 0, 0, -1),
        NodeIndex(-1, 0,  0),
        NodeIndex( 1, 0,  0),
        NodeIndex( 0, 0,  1),
    };

    enum class TrackType : uint8_t
    {
        NONE      ,
        STRAIGHT  ,
        CORNER    ,
        T_JUNCTION,
        CROSS
    };

    struct TrackNode;

    struct Edge
    {
        TrackNode* from;
        TrackNode* to;
        float cost = 1.0f;
        Edge(TrackNode* _from, TrackNode* _to): from(_from), to(_to){ }
    };

    struct TrackNode
    {
        bool is_active;
        entt::entity track_entity;     // Parent tile

        glm::vec3 entry_position;
        glm::vec3 exit_position;

        ConnectDirection entry_dir;
        ConnectDirection exit_dir;

        TrackNode():
            is_active(false),
            track_entity(entt::null),
            entry_dir(ConnectDirection::NONE),
            exit_dir (ConnectDirection::NONE)
        {

        }

        ~TrackNode() = default;

        glm::vec3 exit_world_pos(entt::registry& _registry)
        {
            if(track_entity == entt::null)
                return exit_position; //fallback
            if(_registry.try_get<Transform>(track_entity))
            {
                const Transform& _transform = _registry.get<Transform>(track_entity);
                return _transform.position + exit_position;
            }
            return exit_position; //fallback
        }

        void update_position()
        {
            entry_position = dir_to_local_pos(entry_dir);
            exit_position  = dir_to_local_pos(exit_dir );
        }

        void reset()
        {
            is_active = false;
            entry_dir = ConnectDirection::NONE;
            exit_dir  = ConnectDirection::NONE;
        }
    };

    class Track
    {
    public:
        TrackType type = TrackType::NONE;
        NodeIndex node_index;

        glm::vec3 world_position;

        entt::entity self_entity;
        std::array<entt::entity, 4 > connected_tracks; //-Z/-X/+Z/+X
        std::array<TrackNode   , 12> track_nodes;

        Track()
        {
            connected_tracks.fill(entt::null);
            track_nodes     .fill(TrackNode());
        }
        ~Track() = default;

        void init(uint8_t _bitmask)
        {
            for (TrackNode& _track_node : track_nodes)
            {
                _track_node.reset();
                _track_node.track_entity = self_entity;
            }

            switch (_bitmask)
            {
                case 0b00001001: /*1001*/
                    track_nodes[1].is_active = true;
                    track_nodes[1].entry_dir = ConnectDirection::NORTH;
                    track_nodes[1].exit_dir  = ConnectDirection::SOUTH;

                    track_nodes[7].is_active = true;
                    track_nodes[7].entry_dir = ConnectDirection::SOUTH;
                    track_nodes[7].exit_dir  = ConnectDirection::NORTH;
                    break;
                case 0b00000110: /*0110*/
                    track_nodes[4].is_active = true;
                    track_nodes[4].entry_dir = ConnectDirection::WEST;
                    track_nodes[4].exit_dir  = ConnectDirection::EAST;

                    track_nodes[10].is_active = true;
                    track_nodes[10].entry_dir = ConnectDirection::EAST;
                    track_nodes[10].exit_dir  = ConnectDirection::WEST;
                    break;
                case 0b00000001: /*0001*/
                    track_nodes[1].is_active = true;
                    track_nodes[1].entry_dir = ConnectDirection::SOUTH;
                    track_nodes[1].exit_dir  = ConnectDirection::SOUTH;
                    break;
                case 0b00001000: /*1000*/
                    track_nodes[7].is_active = true;
                    track_nodes[7].entry_dir = ConnectDirection::NORTH;
                    track_nodes[7].exit_dir  = ConnectDirection::NORTH;
                    break;
                case 0b00000100: /*0100*/
                    track_nodes[4].is_active = true;
                    track_nodes[4].entry_dir = ConnectDirection::WEST;
                    track_nodes[4].exit_dir  = ConnectDirection::WEST;
                    break;
                case 0b00000010: /*0010*/
                    track_nodes[10].is_active = true;
                    track_nodes[10].entry_dir = ConnectDirection::EAST;
                    track_nodes[10].exit_dir  = ConnectDirection::EAST;
                    break;
                case 0b00000011: /*0011*/
                    track_nodes[10].is_active = true;
                    track_nodes[10].entry_dir = ConnectDirection::EAST;
                    track_nodes[10].exit_dir  = ConnectDirection::SOUTH;

                    track_nodes[7].is_active = true;
                    track_nodes[7].entry_dir = ConnectDirection::SOUTH;
                    track_nodes[7].exit_dir  = ConnectDirection::NORTH;
                    break;
                case 0b00001100: /*1100*/
                    track_nodes[1].is_active = true;
                    track_nodes[1].entry_dir = ConnectDirection::NORTH;
                    track_nodes[1].exit_dir  = ConnectDirection::WEST;

                    track_nodes[4].is_active = true;
                    track_nodes[4].entry_dir = ConnectDirection::WEST;
                    track_nodes[4].exit_dir  = ConnectDirection::NORTH;
                    break;
                case 0b00000101: /*0101*/
                    track_nodes[4].is_active = true;
                    track_nodes[4].entry_dir = ConnectDirection::WEST;
                    track_nodes[4].exit_dir  = ConnectDirection::SOUTH;

                    track_nodes[7].is_active = true;
                    track_nodes[7].entry_dir = ConnectDirection::SOUTH;
                    track_nodes[7].exit_dir  = ConnectDirection::WEST;
                    break;
                case 0b00001010:
                    track_nodes[1].is_active = true;
                    track_nodes[1].entry_dir = ConnectDirection::NORTH;
                    track_nodes[1].exit_dir  = ConnectDirection::EAST;

                    track_nodes[10].is_active = true;
                    track_nodes[10].entry_dir = ConnectDirection::EAST;
                    track_nodes[10].exit_dir  = ConnectDirection::NORTH;
                    break;
                case 0b00001110:
                    track_nodes[1].is_active = true;
                    track_nodes[1].entry_dir = ConnectDirection::NORTH;
                    track_nodes[1].exit_dir  = ConnectDirection::EAST;

                    track_nodes[4].is_active = true;
                    track_nodes[4].entry_dir = ConnectDirection::WEST;
                    track_nodes[4].exit_dir  = ConnectDirection::NORTH;
                    break;
                case 0b00000111:
                    break;
                case 0b00001101:
                    break;
                case 0b00001011:
                    break;
                case 0b00001111:
                    break;
                default:
                    track_nodes[1].is_active = true;
                    track_nodes[1].entry_dir = ConnectDirection::NORTH;
                    track_nodes[1].exit_dir  = ConnectDirection::SOUTH;

                    track_nodes[7].is_active = true;
                    track_nodes[7].entry_dir = ConnectDirection::SOUTH;
                    track_nodes[7].exit_dir  = ConnectDirection::NORTH;
                    break;
            }

            for (TrackNode& _track : track_nodes)
                _track.update_position();
        }

        void connect(ConnectDirection _dir, entt::entity _target_track)
        {
            connected_tracks[static_cast<uint8_t>(_dir)] = _target_track;
        }

        entt::entity get_connection(ConnectDirection _dir)
        {
            return connected_tracks[static_cast<uint8_t>(_dir)];
        }

        friend std::ostream& operator<<(std::ostream& os, const Track& _track)
        {
            os << "Track : " << _track.node_index.idx << ", " << _track.node_index.idy  << ", " << _track.node_index.idz;
            return os;
        }
    };

    class TrackGraph
    {
    public:
        std::vector<Edge>         edges;
        std::vector<entt::entity> tracks;
        std::unordered_map<NodeIndex, entt::entity> track_map;

        TrackGraph () = default;
        ~TrackGraph() = default;

        void add_track(entt::registry& _registry, Track* _track)
        {
            tracks.push_back(_track->self_entity);
            track_map[_track->node_index] = _track->self_entity;
            update_connections(_registry, _track->node_index);
        }

        void generate_edges(entt::registry& _registry)
        {
            edges.clear();

            for (entt::entity& _e : tracks)
            {
                Track& _track = _registry.get<Track>(_e);

                std::cout << "------------------------" << std::endl;
                std::cout << _track << std::endl;
                std::cout << "------------------------" << std::endl;

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

                    std::cout << "Neighbor : " << dir_to_string(_track_node.exit_dir) << " " << _neighbor_index << std::endl;

                    auto _it = track_map.find(_neighbor_index);
                    if(_it != track_map.end())
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
        }

        void update_connections(entt::registry& _registry, const NodeIndex& _node_idx)
        {
            entt::entity _self = track_map[_node_idx];

            auto& _track = _registry.get<Track>(_self);

            _track.connected_tracks.fill(entt::null);

            for (uint8_t _i = 0; _i < VICINITY_4_DIR.size(); ++_i)
            {
                NodeIndex _neighbor = _node_idx + VICINITY_4_DIR[_i];

                auto it = track_map.find(_neighbor);
                if (it != track_map.end())
                {
                    entt::entity neighbor_entity = it->second;

                    _track.connected_tracks[_i] = neighbor_entity;

                    auto& neighbor_track = _registry.get<Track>(neighbor_entity);

                    neighbor_track.connected_tracks[OPPOSITE_DIR[_i]] = _self;
                }
            }
        }

        std::vector<glm::vec3> get_track_positions(entt::registry& _registry)
        {
            std::vector<glm::vec3> _positions;
            for (entt::entity& _e : tracks)
            {
                Track&     _track     = _registry.get<Track>(_e);
                Transform& _transform = _registry.get<Transform>(_e);

                _positions.push_back(_transform.position);
            }

            return _positions;
        }

        //Remove?
        float heuristic_dist(entt::registry& _registry, TrackNode* _a, TrackNode* _b)
        {
            glm::vec3 _exit_a = _a->exit_world_pos(_registry);
            glm::vec3 _exit_b = _b->exit_world_pos(_registry);
            glm::vec3 _dist = _exit_a - _exit_b;
            return std::sqrt(_dist.x*_dist.x + _dist.y*_dist.y + _dist.z*_dist.z);
        }

        std::unordered_map<TrackNode*, std::vector<TrackNode*>> build_adjacency_from_edges()
        {
            std::unordered_map<TrackNode*, std::vector<TrackNode*>> _adj;

            _adj.reserve(edges.size() * 2);

            for(Edge& _e : edges)
            {
                if(!_e.from || !_e.to)
                    continue;
                _adj[_e.from].push_back(_e.to);
            }
            return _adj;
        }

        std::vector<TrackNode*> a_star_search(entt::registry& reg, TrackNode* _start, TrackNode* _goal, const std::unordered_map<TrackNode*, std::vector<TrackNode*>>& adjacency)
        {
            if (!_start || !_goal)
                return {};

            // Min-heap keyed by f = g + h. store pair<fscore, node>
            using PQItem = std::pair<float, TrackNode*>;
            struct Compare {
                bool operator()(PQItem const& a, PQItem const& b) const { return a.first > b.first; } // min-heap
            };

            std::priority_queue<PQItem, std::vector<PQItem>, Compare> open_queue;
            std::unordered_set<TrackNode*> open_set;
            std::unordered_set<TrackNode*> closed_set;

            std::unordered_map<TrackNode*, TrackNode*> came_from;
            std::unordered_map<TrackNode*, float> g_score;
            std::unordered_map<TrackNode*, float> f_score;

            const float INF = std::numeric_limits<float>::infinity();

            g_score[_start] = 0.0f;
            f_score[_start] = heuristic_dist(reg, _start, _goal);

            open_queue.push({ f_score[_start], _start });

            open_set.insert(_start);

            while (!open_queue.empty())
            {
                TrackNode* current = open_queue.top().second;
                open_queue.pop();

                open_set.erase(current);

                if (current == _goal)
                {
                    std::vector<TrackNode*> path;
                    TrackNode* cur = _goal;
                    while (cur)
                    {
                        path.push_back(cur);
                        auto it = came_from.find(cur);
                        if (it == came_from.end()) break;
                        cur = it->second;
                    }
                    std::reverse(path.begin(), path.end());
                    return path;
                }

                closed_set.insert(current);

                auto neigh_it = adjacency.find(current);
                if (neigh_it == adjacency.end())
                    continue;

                for (TrackNode* neighbor : neigh_it->second)
                {
                    if (!neighbor)
                        continue;

                    if (closed_set.find(neighbor) != closed_set.end())
                        continue;

                    float tentative_g = g_score[current] +
                        glm::distance(
                            current ->exit_world_pos(reg),
                            neighbor->exit_world_pos(reg)
                            );

                    auto g_it = g_score.find(neighbor);
                    if (g_it == g_score.end() || tentative_g < g_it->second)
                    {
                        // This path to neighbor is better
                        came_from[neighbor] = current;
                        g_score[neighbor] = tentative_g;
                        float h = heuristic_dist(reg, neighbor, _goal);
                        f_score[neighbor] = tentative_g + h;

                        // push to open if not already there
                        if (open_set.find(neighbor) == open_set.end())
                        {
                            open_queue.push({ f_score[neighbor], neighbor });
                            open_set.insert(neighbor);
                        }
                    }
                }
            }
            return {};
        }
        //-----

        void print_edges(entt::registry& registry)
        {
            std::cout << std::endl;
            std::cout << "=== TrackGraph Edges ===\n";
            for (const Edge& edge : edges)
            {
                if (!edge.from || !edge.to)
                    continue;

                // Optionally, get entity IDs or track directions
                entt::entity fromEntity = edge.from->track_entity;
                entt::entity toEntity   = edge.to  ->track_entity;

                if (Track* _from_t = registry.try_get<Track>(fromEntity))
                    if (Track* _to_t = registry.try_get<Track>(toEntity))
                        std::cout << "Edge: [from] " << _from_t->node_index << " [to] " << _to_t->node_index << "\n";
            }

            std::cout << std::endl;
        }
    };
}

#endif
