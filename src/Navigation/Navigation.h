#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <Components/Camera.h>

#include "../PCH.h"
#include "Components/Component.h"
#include "Components/Transform.h"

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

        glm::vec3 entry_world_pos(entt::registry& _registry)
        {
            if(track_entity == entt::null)
                return entry_position; //fallback
            if(_registry.try_get<Component::Transform>(track_entity))
            {
                const Component::Transform& _transform = _registry.get<Component::Transform>(track_entity);
                return _transform.position + entry_position;
            }
            return entry_position; //fallback
        }

        glm::vec3 exit_world_pos(entt::registry& _registry)
        {
            if(track_entity == entt::null)
                return exit_position; //fallback
            if(_registry.try_get<Component::Transform>(track_entity))
            {
                const Component::Transform& _transform = _registry.get<Component::Transform>(track_entity);
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
                case 0b00001110: /*1110*/
                    track_nodes[1].is_active = true;
                    track_nodes[1].entry_dir = ConnectDirection::NORTH;
                    track_nodes[1].exit_dir  = ConnectDirection::EAST;

                    track_nodes[4].is_active = true;
                    track_nodes[4].entry_dir = ConnectDirection::WEST;
                    track_nodes[4].exit_dir  = ConnectDirection::NORTH;
                    break;
                case 0b00000111: /*0111*/
                    track_nodes[4] .is_active = true;
                    track_nodes[4].entry_dir = ConnectDirection::WEST;
                    track_nodes[4].exit_dir  = ConnectDirection::SOUTH;

                    track_nodes[10].is_active = true;
                    track_nodes[10].entry_dir = ConnectDirection::EAST;
                    track_nodes[10].exit_dir  = ConnectDirection::SOUTH;

                    track_nodes[7] .is_active = true;
                    track_nodes[7].entry_dir = ConnectDirection::SOUTH;
                    track_nodes[7].exit_dir  = ConnectDirection::WEST;
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

        TrackNode* get_track_node(ConnectDirection _entry_dir, ConnectDirection _exit_dir)
        {
            if (_entry_dir == ConnectDirection::NORTH && _exit_dir == ConnectDirection::WEST)
                return &track_nodes[2];
            if (_entry_dir == ConnectDirection::NORTH && _exit_dir == ConnectDirection::SOUTH)
                return &track_nodes[1];
            if (_entry_dir == ConnectDirection::NORTH && _exit_dir == ConnectDirection::EAST)
                return &track_nodes[0];

            if (_entry_dir == ConnectDirection::WEST && _exit_dir == ConnectDirection::NORTH)
                return &track_nodes[3];
            if (_entry_dir == ConnectDirection::WEST && _exit_dir == ConnectDirection::EAST)
                return &track_nodes[4];
            if (_entry_dir == ConnectDirection::WEST && _exit_dir == ConnectDirection::SOUTH)
                return &track_nodes[5];

            if (_entry_dir == ConnectDirection::EAST && _exit_dir == ConnectDirection::NORTH)
                return &track_nodes[11];
            if (_entry_dir == ConnectDirection::EAST && _exit_dir == ConnectDirection::WEST)
                return &track_nodes[10];
            if (_entry_dir == ConnectDirection::EAST && _exit_dir == ConnectDirection::SOUTH)
                return &track_nodes[9];

            if (_entry_dir == ConnectDirection::SOUTH && _exit_dir == ConnectDirection::WEST)
                return &track_nodes[6];
            if (_entry_dir == ConnectDirection::SOUTH && _exit_dir == ConnectDirection::NORTH)
                return &track_nodes[7];
            if (_entry_dir == ConnectDirection::SOUTH && _exit_dir == ConnectDirection::EAST)
                return &track_nodes[8];

            return nullptr;
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

        Track* try_get_track_at(entt::registry& _registry, NodeIndex _node_index)
        {
            auto _it = track_map.find(_node_index);
            if (_it != track_map.end())
            {
                auto& _e = _it->second;
                if(Track* _found_track = _registry.try_get<Track>(_e))
                {
                    std::cout << "Found track at : " << _node_index << std::endl;
                    return _found_track;
                }
            }
            return nullptr;
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
                Component::Transform& _transform = _registry.get<Component::Transform>(_e);
                _positions.push_back(_transform.position);
            }
            return _positions;
        }
    };
}

#endif
