#ifndef SHADER_H
#define SHADER_H

#include "PCH.h"

enum class ShaderType
{
    NONE     = -1,
    VERTEX   = 0,
    GEOMETRY = 1,
    FRAGMENT = 2
};

class Shader
{
public:
    GLuint id;

    Shader(std::filesystem::path _path);
    Shader();
    ~Shader();

    void use();

    void set_mat4_uniform_model                 (glm::mat4 _model              ) const;
    void set_mat4_uniform_view                  (glm::mat4 _view               ) const;
    void set_mat4_uniform_projection            (glm::mat4 _projection         ) const;

    void block_bind(const std::string& _block_name, uint32_t _bind_point)
    {
        unsigned int uniform_block_index  = glGetUniformBlockIndex(id, _block_name.c_str());
        glUniformBlockBinding(id, uniform_block_index, _bind_point);
    }

    friend std::ostream& operator<<(std::ostream& _os, Shader& _shader)
    {
        _os << "[ Vertex shader source ]" << '\n';
        _os << _shader.m_vertex_src << '\n';
        _os << '\n';
        _os << "[ Fragment shader source ]" << '\n';
        _os << _shader.m_fragment_src << '\n';
        return _os;
    }

    GLuint create_shader(const std::string& _vertex_src, const std::string& _geometry_src, const std::string& _fragment_src);
    GLuint compile_shader(GLenum _shader_type, const std::string &_shader_src);

private:
    std::string m_vertex_src;
    std::string m_geometry_src;
    std::string m_fragment_src;

    GLint m_uniform_loc_model;
    GLint m_uniform_loc_view;
    GLint m_uniform_loc_projection;
};
#endif 
