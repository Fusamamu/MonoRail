#include "PerlinNoise.h"

PerlinNoise::PerlinNoise(unsigned int seed) {
    p.resize(256);
    std::iota(p.begin(), p.end(), 0);

    std::default_random_engine engine(seed);
    std::shuffle(p.begin(), p.end(), engine);

    // Duplicate to avoid wrapping
    p.insert(p.end(), p.begin(), p.end());
}

float PerlinNoise::noise(float x, float y) const {
    // Find unit grid cell containing point
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    // Relative coords inside cell
    x -= floor(x);
    y -= floor(y);

    // Fade curves
    float u = fade(x);
    float v = fade(y);

    // Hash coords of the 4 corners
    int aa = p[p[X    ] + Y    ];
    int ab = p[p[X    ] + Y + 1];
    int ba = p[p[X + 1] + Y    ];
    int bb = p[p[X + 1] + Y + 1];

    // Blend results
    float res = lerp(v,
        lerp(u, grad(aa, x, y), grad(ba, x - 1, y)),
        lerp(u, grad(ab, x, y - 1), grad(bb, x - 1, y - 1))
    );
    return (res + 1.0f) * 0.5f; // normalize to [0,1]
}

float PerlinNoise::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PerlinNoise::lerp(float t, float a, float b) {
    return a + t * (b - a);
}

float PerlinNoise::grad(int hash, float x, float y) {
    int h = hash & 7; // 8 directions
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}
