#include "framebuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(unsigned int width, unsigned int height, bool use_depth)
    : m_width(width), m_height(height), m_has_depth(use_depth), m_fbo(0), m_color_texture(0), m_depth_rbo(0)
{
}

FrameBuffer::~FrameBuffer() {
    cleanup();
}

void FrameBuffer::init()
{
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Create color texture
    glGenTextures(1, &m_color_texture);
    glBindTexture(GL_TEXTURE_2D, m_color_texture);

    glTexImage2D   (GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_color_texture, 0);

    // Optional depth and stencil renderbuffer
    if (m_has_depth)
    {
        glGenRenderbuffers(1, &m_depth_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depth_rbo);

        glRenderbufferStorage    (GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER , GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depth_rbo);
    }

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::cleanup() {
    if (m_color_texture) {
        glDeleteTextures(1, &m_color_texture);
        m_color_texture = 0;
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
    glViewport(0, 0, m_width, m_height);
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

GLuint FrameBuffer::get_fbo() const
{
    return m_fbo;
}
