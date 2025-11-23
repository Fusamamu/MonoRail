#include "FrameBuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, bool use_depth):
    m_width(width),
    m_height(height),
    m_has_depth(use_depth),
    m_fbo(0),
    m_color_texture(0),
    m_depth_texture(0),
    m_depth_rbo(0)
{
}

FrameBuffer::~FrameBuffer() {
    cleanup();
}

void FrameBuffer::init()
{
    glGenFramebuffers(1, &m_fbo);
}

void FrameBuffer::attach_color_texture()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_color_texture);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);

    glTexImage2D   (GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::attach_depth_texture()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glGenTextures(1, &m_depth_texture);
    glBindTexture(GL_TEXTURE_2D, m_depth_texture);

    glTexImage2D   (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_width, m_height, 0,GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::cleanup()
{
    if (m_color_texture) {
        glDeleteTextures(1, &m_color_texture);
        m_color_texture = 0;
    }

    if (m_depth_texture) {
        glDeleteTextures(1, &m_depth_texture);
        m_depth_texture = 0;
    }

    if (m_depth_rbo && m_has_depth) {
        glDeleteRenderbuffers(1, &m_depth_rbo);
        m_depth_rbo = 0;
    }

    if (m_fbo) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
}

void FrameBuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void FrameBuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::resize(unsigned int new_width, unsigned int new_height)
{
    m_width  = new_width;
    m_height = new_height;

    cleanup();
    init();
}

GLuint FrameBuffer::get_color_texture() const
{
    return m_color_texture;
}

GLuint FrameBuffer::get_depth_texture() const
{
    return m_depth_texture;
}

GLuint FrameBuffer::get_fbo() const
{
    return m_fbo;
}
