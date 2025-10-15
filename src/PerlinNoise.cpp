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

GLuint PerlinNoise::generate_perlin_texture(int _width, int _height, float _scale, unsigned int _seed)
{
    std::vector<float> data(_width * _height);

    for (int y = 0; y < _height; ++y)
    {
        for (int x = 0; x < _width; ++x)
        {
            float fx = (float)x / _width  * _scale;
            float fy = (float)y / _height * _scale;

            float n = noise(fx, fy);

            data[y * _width + x] = n;
        }
    }

    GLuint _texture_id;
    glGenTextures(1, &_texture_id);
    glBindTexture(GL_TEXTURE_2D, _texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, _width, _height, 0, GL_RED, GL_FLOAT, data.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    return _texture_id;
}