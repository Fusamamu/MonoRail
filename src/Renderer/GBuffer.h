#ifndef G_BUFFER_H
#define G_BUFFER_H

namespace MUG::Renderer
{
    class GBuffer
    {
    public:
        GBuffer(unsigned int _width, unsigned int _height);
        GBuffer();
        ~GBuffer();

        void init  ();
        void resize(unsigned int _new_width, unsigned int _new_height);
        void bind  () const;
        void unbind() const;

        GLuint add_color_attachment(GLenum internal_format);
        void add_depth_attachment();

        GLuint get_fbo() const { return m_fbo; }
        GLuint get_color_texture(int index) const { return m_color_textures[index]; }
        GLuint get_depth_texture() const { return m_depth_texture; }

    private:
        void destroy();
        void complete();

        unsigned int m_width, m_height;
        GLuint m_fbo;

        std::vector<GLuint> m_color_textures;
        GLuint m_depth_texture;
    };
};

#endif //GBUFFER_H
