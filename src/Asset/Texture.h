#ifndef TEXTURE_H
#define TEXTURE_H

#include "Math/MMath.h"
#include <vector>
#include <GL/glew.h>

struct Texture
{
    GLuint texture_id;
    GLenum texture_target = GL_TEXTURE_2D;
    int width, height, nrComponents;
    unsigned char* p_data;

    Texture() = default;
    ~Texture() = default;

    void bind(int _slot = 0) const;
    void generate_texture(int _width, int _height, const std::vector<float>& _data);
};

struct Texture3D
{
    GLuint id;
    GLenum texture_target = GL_TEXTURE_3D;
    int width, height, depth;

    void populate(const std::vector<uint8_t>& _data);
    void generate(int _resolution);
    void generate_texture(int _w, int _h, int _d);
};

#endif
