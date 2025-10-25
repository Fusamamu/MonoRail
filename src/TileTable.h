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
            case 0b00001001:
                return "r_0100_0010";
            case 0b00000110:
                return "r_0001_1000";
            case 0b00000001:
                return "r_0000_0001";
            case 0b00001000:
                return "r_0000_1000";
            case 0b00000100:
                return "r_0000_0100";
            case 0b00000010:
                return "r_0000_0010";
            case 0b00000011:
                return "r_0000_0011";
            case 0b00001100:
                return "r_0000_1100";
            case 0b00000101:
                return "r_0000_0101";
            case 0b00001010:
                return "r_0000_1010";
            case 0b00001110:
                return "r_0000_1110";
            case 0b00000111:
                return "r_0000_0111";
            case 0b00001101:
                return "r_0000_1101";
            case 0b00001011:
                return "r_0000_1011";
            case 0b00001111:
                return "r_0000_1111";
            default:
                return "r_0100_0010";
        }
    }
};

#endif
