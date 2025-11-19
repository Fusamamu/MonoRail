#ifndef MMATH_H
#define MMATH_H

#include <vector>

namespace EASE
{
    float linear(float t);
    float smoothstep(float t);
    float ease_in(float t);
    float ease_out(float t);
    float ease_in_out(float t);
    float ease_out_back(float t);

}

namespace Util
{
    glm::vec3 world_to_screen(
        const glm::vec3& worldPos,
        const glm::mat4& view,
        const glm::mat4& proj,
        int screenWidth,
        int screenHeight);
}

namespace PROCGEN
{
    inline float fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    inline float lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    inline float grad(int hash, float x, float y) {
        int h = hash & 7; // 8 directions
        float u = h < 4 ? x : y;
        float v = h < 4 ? y : x;
        return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
    }

    inline float noise(float x, float y, unsigned int _seed)
    {
        std::vector<int> p;

        p.resize(256);
        std::iota(p.begin(), p.end(), 0);

        std::default_random_engine engine(_seed);
        std::shuffle(p.begin(), p.end(), engine);

        // Duplicate to avoid wrapping
        p.insert(p.end(), p.begin(), p.end());

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

    inline std::vector<float> generate_perlin_noise(uint32_t _width, uint32_t _height, float _scale)
    {
        std::vector<float> _data(_width * _height);

        for (int y = 0; y < _height; ++y)
        {
            for (int x = 0; x < _width; ++x)
            {
                float fx = (float)x / _width  * _scale;
                float fy = (float)y / _height * _scale;

                float n = noise(fx, fy, 2025);

                _data[y * _width + x] = n;
            }
        }

        return _data;
    }
}

#endif
