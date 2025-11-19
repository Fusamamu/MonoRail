#ifndef NODE3D_H
#define NODE3D_H


enum class TileType : uint8_t
{
    NONE   = 0,
    GROUND = 1 << 0,
};

struct NodeIndex
{
    int32_t idx;
    int32_t idy;
    int32_t idz;

    NodeIndex(int32_t _idx, int32_t _idy, int32_t _idz) : idx(_idx), idy(_idy), idz(_idz) {}
    NodeIndex() = default;
    ~NodeIndex() = default;

    NodeIndex operator+(const NodeIndex& _other) const{
        return NodeIndex(idx + _other.idx, idy + _other.idy, idz + _other.idz);
    }

    bool operator==(const NodeIndex& _other) const{
        return idx == _other.idx && idy == _other.idy && idz == _other.idz;
    }

    friend std::ostream& operator<<(std::ostream& os, const NodeIndex& _other)
    {
        os << _other.idx << ", " << _other.idy << ", " << _other.idz;
        return os;
    }
};


struct Node3D
{
    int32_t idx;
    int32_t idy;
    int32_t idz;

    uint8_t bit;

    TileType type = TileType::NONE;

    std::array<entt::entity, 8> corner_nodes;

    bool is_occupied = false;

    Node3D() = default;
    Node3D(uint32_t x, uint32_t y, uint32_t z)
        : idx(x), idy(y), idz(z) {}

    NodeIndex to_node_index() const{
        return NodeIndex(idx, idy, idz);
    }
    NodeIndex to_node_index(int32_t _idx, int32_t _idy, int32_t _idz){
        return NodeIndex(idx + _idx, idy + _idy, idz + _idz);
    }

    uint8_t to_bitmask(entt::registry& _registry)
    {
        uint8_t mask = 0;
        for (size_t i = 0; i < corner_nodes.size(); ++i)
        {
            if (corner_nodes[i] != entt::null)
            {
                Node3D _node = _registry.get<Node3D>(corner_nodes[i]);
                if (_node.is_occupied)
                    mask |= (1 << (7 - i));
            }
        }

        bit = mask;
        return mask;
    }

    void print() const{ std::cout << *this; }

    friend std::ostream& operator<<(std::ostream& os, const Node3D& node)
    {
        os << "Node3D("
           << node.idx << ", "
           << node.idy << ", "
           << node.idz << ") "
           << "occupied=" << std::boolalpha << node.is_occupied;

        uint8_t mask = 0;
        for (size_t i = 0; i < node.corner_nodes.size(); ++i)
        {
            if (node.corner_nodes[i] != entt::null) // occupied
                mask |= (1 << i);
        }

        os << " corner_mask=0b" << std::bitset<8>(mask);

        return os;
    }
};

namespace std
{
    template<>
    struct hash<NodeIndex>
    {
        std::size_t operator()(const NodeIndex& k) const noexcept
        {
            return (std::hash<int>()(k.idx) * 73856093) ^ (std::hash<int>()(k.idz) * 19349663);
        }
    };
}



#endif //NODE3D_H
