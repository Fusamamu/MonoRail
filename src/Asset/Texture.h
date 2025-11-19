#ifndef TEXTURE_H
#define TEXTURE_H

#include "Math/MMath.h"

struct Texture
{
    GLuint texture_id;
    GLenum texture_target = GL_TEXTURE_2D;
    int width, height, nrComponents;
    unsigned char* p_data;

    void bind(int _slot = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + _slot);
        glBindTexture(texture_target, texture_id);
    }

    void generate_texture(int _width , int _height, const std::vector<float>& _data)
    {
        width        = _width;
        height       = _height;
        nrComponents = GL_RGB;

        glGenTextures   (1, &texture_id);
        glBindTexture   (GL_TEXTURE_2D, texture_id);
        glTexImage2D    (GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_FLOAT, _data.data());
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_CLAMP_TO_EDGE);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture   (GL_TEXTURE_2D, 0);
    }
};

struct Texture3D
{
    GLuint id;
    GLenum texture_target = GL_TEXTURE_3D;

    int width, height, depth;

    void populate(const std::vector<uint8_t>& _data)
    {
        glBindTexture(GL_TEXTURE_3D, id);
        glTexSubImage3D(GL_TEXTURE_3D, 0, 
                        0, 0, 0, 
                        width, height, depth,
                        GL_RED, GL_UNSIGNED_BYTE,
                        _data.data());
    }

    void generate(int _resolution)
    {
        width  = _resolution;
        height = _resolution;
        depth  = _resolution;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_3D, id);

        //glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, _resolution, _resolution, _resolution, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, _resolution, _resolution, _resolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
    
    void generate_texture(int _w, int _h, int _d)
    {
        width  = _w;
        height = _h;
        depth  = _d;

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_3D, id);

        glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, _w, _h, _d, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
};
#endif
