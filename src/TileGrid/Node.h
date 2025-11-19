//
// Created by Sukum Duangpattra on 19/11/2025 AD.
//

#ifndef NODE_H
#define NODE_H


struct Node
{
    std::string name;

    bool is_active = false;
    bool is_dirty  = false;
    bool is_static = false;

    Node() = default;
    Node(const std::string& name) : name(name) {}
};

struct Tile
{
    uint32_t idx, idy;
    Tile() = default;
    Tile(uint32_t x, uint32_t y) : idx(x), idy(y) {}
};




#endif //NODE_H
