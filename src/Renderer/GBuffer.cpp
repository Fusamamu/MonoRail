#include "GBuffer.h"

namespace MUG::Renderer
{
    GBuffer::GBuffer(unsigned int _width, unsigned int _height)
        : m_width(_width), m_height(_height), m_fbo(0), m_depth_texture(0)
    {
    }

    GBuffer::GBuffer()
    {

    }

    GBuffer::~GBuffer()
    {
        destroy();
    }

    void GBuffer::init()
    {
        destroy();
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }

    void GBuffer::destroy()
    {
        if (!m_color_textures.empty())
        {
            glDeleteTextures((GLsizei)m_color_textures.size(), m_color_textures.data());
            m_color_textures.clear();
        }

        if (m_depth_texture)
        {
            glDeleteTextures(1, &m_depth_texture);
            m_depth_texture = 0;
        }

        if (m_fbo) {
            glDeleteFramebuffers(1, &m_fbo);
            m_fbo = 0;
        }
    }

    GLuint GBuffer::add_color_attachment(GLenum internal_format)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format,
                     m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        GLenum attachment = GL_COLOR_ATTACHMENT0 + (GLenum)m_color_textures.size();
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex, 0);

        m_color_textures.push_back(tex);

        complete();
        return tex;
    }

    void GBuffer::add_depth_attachment()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        glGenTextures(1, &m_depth_texture);
        glBindTexture(GL_TEXTURE_2D, m_depth_texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
                     m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, m_depth_texture, 0);

        complete();
    }

    void GBuffer::complete()
    {
        std::vector<GLenum> attachments;
        for (int i = 0; i < (int)m_color_textures.size(); i++)
            attachments.push_back(GL_COLOR_ATTACHMENT0 + i);

        if (!attachments.empty())
            glDrawBuffers((GLsizei)attachments.size(), attachments.data());

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR: GBuffer incomplete\n";
    }

    void GBuffer::bind  () const { glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); }
    void GBuffer::unbind() const { glBindFramebuffer(GL_FRAMEBUFFER,     0); }

    void GBuffer::resize(unsigned int _new_width, unsigned int _new_height)
    {
        m_width  = _new_width;
        m_height = _new_height;
        init();
    }
}