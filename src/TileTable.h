#ifndef TILETABLE_H
#define TILETABLE_H

#include "PCH.h"

class TileTable
{
public:
    TileTable() = default;
    ~TileTable() = default;

    std::string get_tile(uint8_t _bit)
    {
        switch (_bit)
        {
            case 0b01000010:
                return "r_0100_0010";
            case 0b00011000:
                return "r_0001_1000";
            default:
                return "r_0100_0010";
        }
    }
};

#endif
