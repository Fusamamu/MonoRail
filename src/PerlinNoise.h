#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include "PCH.h"

class PerlinNoise {
public:
    explicit PerlinNoise(unsigned int seed = 2025);
    float noise(float x, float y) const;
private:
    std::vector<int> p;

    static float fade(float t);
    static float lerp(float t, float a, float b);
    static float grad(int hash, float x, float y);
};

#endif //PERLINNOISE_H
