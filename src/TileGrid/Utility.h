#ifndef TILE_GRID_UTILITY_H
#define TILE_GRID_UTILITY_H

#include "TileGrid/Grid3D.h"

namespace TileGrid
{
    static inline float heuristic_manhattan(NodeIndex _node_a, NodeIndex _node_b)
    {
        return std::abs((int)_node_a.idx - (int)_node_b.idx)
            + std::abs((int)_node_a.idy - (int)_node_b.idy)
            + std::abs((int)_node_a.idz - (int)_node_b.idz);
    }
}

#endif
