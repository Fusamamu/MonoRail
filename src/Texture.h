#ifndef TEXTURE_H
#define TEXTURE_H

#include "PCH.h"
#include "MMath.h"

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
#endif
