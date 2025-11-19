#ifndef UTILITY_H
#define UTILITY_H

namespace Utility
{
    inline float randomFloat(float _min, float _max)
    {
        return _min + static_cast<float>(rand()) / RAND_MAX * (_max - _min);
    }

    inline std::string to_formatted_name(uint8_t _value)
    {
        std::string bits = std::bitset<8>(_value).to_string();
        bits.insert(4, "_");
        return "c_" + bits;
    }
}

#endif //UTILITY_H
