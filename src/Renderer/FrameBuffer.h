#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../PCH.h"

class FrameBuffer {
public:
    FrameBuffer(unsigned int width, unsigned int height, bool use_depth = true);
    FrameBuffer() = default;
    ~FrameBuffer();

    void init();
    void bind() const;
    void unbind() const;

    void resize(unsigned int new_width, unsigned int new_height);

    void attach_color_texture();
    void attach_depth_texture();

    GLuint get_color_texture() const;
    GLuint get_depth_texture() const;
    GLuint get_fbo() const;

private:
    GLuint m_fbo;
    GLuint m_color_texture;
    GLuint m_depth_texture;
    GLuint m_depth_rbo;

    unsigned int m_width;
    unsigned int m_height;

    bool m_has_depth;

    void cleanup();
};

#endif // FRAMEBUFFER_H
