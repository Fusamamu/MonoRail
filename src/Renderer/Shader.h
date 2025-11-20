#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    void set_mat4_uniform_model                 (glm::mat4 _model     ) const;
    void set_mat4_uniform_view                  (glm::mat4 _view      ) const;
    void set_mat4_uniform_projection            (glm::mat4 _projection) const;

    void set_int  (const std::string& _name, int   _value)             const;
    void set_float(const std::string& _name, float _value)             const;
    void set_vec2 (const std::string& _name, const glm::vec2& _value ) const;
    void set_vec3 (const std::string& _name, const glm::vec3& _value ) const;
    void set_vec4 (const std::string& _name, const glm::vec4& _value ) const;
    void set_mat4 (const std::string& _name, const glm::mat4& _matrix) const;

    void block_bind(const std::string& _block_name, uint32_t _bind_point);

    friend std::ostream& operator<<(std::ostream& _os, Shader& _shader);

    GLuint create_shader (const std::string& _vertex_src, const std::string& _geometry_src, const std::string& _fragment_src);
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
