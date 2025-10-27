#include "Navigation.h"

namespace NAV
{
    ConnectDirection opposite_dir(ConnectDirection dir)
    {
        switch(dir)
        {
            case ConnectDirection::NORTH: return ConnectDirection::SOUTH;
            case ConnectDirection::SOUTH: return ConnectDirection::NORTH;
            case ConnectDirection::WEST : return ConnectDirection::EAST ;
            case ConnectDirection::EAST : return ConnectDirection::WEST ;
        }
        return ConnectDirection::NONE;
    }
}