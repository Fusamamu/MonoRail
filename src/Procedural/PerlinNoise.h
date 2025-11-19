#ifndef PERLINNOISE_H
#define PERLINNOISE_H

namespace Procgen
{
    class PerlinNoise {
    public:
        explicit PerlinNoise(unsigned int seed = 2025);
        float noise(float x, float y) const;

        GLuint generate_perlin_texture(int _width, int _height, float _scale, unsigned int _seed);

        std::vector<float> generate_perlin_data(int _width, int _height, float _scale, unsigned int _seed)
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

            return data;
        }
    private:
        std::vector<int> p;

        static float fade(float t);
        static float lerp(float t, float a, float b);
        static float grad(int hash, float x, float y);

    };
}

#endif //PERLINNOISE_H
