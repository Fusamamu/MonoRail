#include "Shader.h"

Shader::Shader(std::filesystem::path _path)
{
    if(!exists(_path))
    {
        std::cout << "File path " << _path << "at absolute location " << absolute(_path) << " does not exists\n";
        return;
    }

    std::ifstream _input_file(_path);
    
    if(!_input_file.good())
    {
        std::cout << "File does not exist!" << std::endl;
        return;
    }
    if(!_input_file.is_open())
    {
        std::cout << "Input file not open!" << std::endl;
        return;
    }

    ShaderType _shader_type = ShaderType::NONE;

    std::stringstream _ss[3];
    std::string       _line;

    while(std::getline(_input_file, _line)) 
    {
        if(_line.find("#shader vertex"  ) != std::string::npos)
        {
            _shader_type = ShaderType::VERTEX;
            continue;
        }
        if(_line.find("#shader geometry") != std::string::npos)
        {
            _shader_type = ShaderType::GEOMETRY;
            continue;
        }
        if(_line.find("#shader fragment") != std::string::npos)
        {
            _shader_type = ShaderType::FRAGMENT;
            continue;
        }
        
        int _target_index = (int)(_shader_type);
        if(_target_index == -1)
            continue;

        _ss[_target_index] << _line << '\n';
    }

    m_vertex_src   = _ss[(int)(ShaderType::VERTEX  )].str();
    m_geometry_src = _ss[(int)(ShaderType::GEOMETRY)].str();
    m_fragment_src = _ss[(int)(ShaderType::FRAGMENT)].str(); 

    _input_file.close();

    id = create_shader(m_vertex_src, m_geometry_src, m_fragment_src);

    m_uniform_loc_model      = glGetUniformLocation(id, "model");
    m_uniform_loc_view       = glGetUniformLocation(id, "view" );
    m_uniform_loc_projection = glGetUniformLocation(id, "proj" );

    int _block_count;
    glGetProgramiv(id, GL_ACTIVE_UNIFORM_BLOCKS, &_block_count);

    for (int i = 0; i < _block_count; i++)
    {
        char _block_name[128];
        glGetActiveUniformBlockName(id, i, 128, nullptr, _block_name);

        if (strcmp(_block_name, "CameraData"           ) == 0) block_bind(_block_name, 0);
        if (strcmp(_block_name, "DirectionalLightBlock") == 0) block_bind(_block_name, 1);
        if (strcmp(_block_name, "FogDataBlock"         ) == 0) block_bind(_block_name, 2);
    }
}

Shader::Shader()
{
}

Shader::~Shader()
{
    glDeleteProgram(id);
}

void Shader::use()
{
    glUseProgram(id);
}

void Shader::set_mat4_uniform_model(glm::mat4 _model) const
{
    glUniformMatrix4fv(m_uniform_loc_model, 1, GL_FALSE, glm::value_ptr(_model));
}

void Shader::set_mat4_uniform_view(glm::mat4 _view) const
{
    glUniformMatrix4fv(m_uniform_loc_view, 1, GL_FALSE, glm::value_ptr(_view));
}

void Shader::set_mat4_uniform_projection(glm::mat4 _projection) const
{
    glUniformMatrix4fv(m_uniform_loc_projection, 1, GL_FALSE, glm::value_ptr(_projection));
}

void Shader::set_int(const std::string& _name, int _value) const
{
    glUniform1i(glGetUniformLocation(id, _name.c_str()), _value);
}

void Shader::set_float(const std::string& _name, float _value) const
{
    glUniform1f(glGetUniformLocation(id, _name.c_str()), _value);
}

void Shader::set_vec2(const std::string& _name, const glm::vec2& _value) const
{
    glUniform2f(glGetUniformLocation(id,_name.c_str()), (float)_value.x, (float)_value.y);
}

void Shader::set_vec3(const std::string& _name, const glm::vec3& _value) const
{
    glUniform3f(glGetUniformLocation(id,_name.c_str()), _value.x, _value.y, _value.z);
}

void Shader::set_vec4(const std::string& _name, const glm::vec4& _value) const
{
    glUniform4f(glGetUniformLocation(id, _name.c_str()),_value.x, _value.y, _value.z, _value.w);
}

void Shader::set_mat4(const std::string& _name, const glm::mat4& _matrix) const
{
    GLint _loc = glGetUniformLocation(id, _name.c_str());
    glUniformMatrix4fv(_loc, 1, GL_FALSE, glm::value_ptr(_matrix));
}

void Shader::block_bind(const std::string& _block_name, uint32_t _bind_point)
{
    unsigned int _uniform_block_index  = glGetUniformBlockIndex(id, _block_name.c_str());
    if (_uniform_block_index == GL_INVALID_INDEX)
        return;
    glUniformBlockBinding(id, _uniform_block_index, _bind_point);
}

GLuint Shader::create_shader
(
    const std::string& _vertex_src, 
    const std::string& _geometry_src, 
    const std::string& _fragment_src
)
{
    GLuint _vertexShader   = compile_shader(GL_VERTEX_SHADER  , _vertex_src  );
    GLuint _fragmentShader = compile_shader(GL_FRAGMENT_SHADER, _fragment_src);

    GLuint _shaderProgram = glCreateProgram();
    glAttachShader(_shaderProgram, _vertexShader  );
    glAttachShader(_shaderProgram, _fragmentShader);

    if(!_geometry_src.empty())
    {
        GLuint _geometryShader = compile_shader(GL_GEOMETRY_SHADER, _geometry_src);
        glAttachShader(_shaderProgram, _geometryShader);
    }

    glLinkProgram(_shaderProgram);

    GLint _success;
    glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &_success);
    if (!_success) {
        char infoLog[512];
        glGetProgramInfoLog(_shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Program Linking Error: " << infoLog << std::endl;
    }

    glDeleteShader(_vertexShader);
    glDeleteShader(_fragmentShader);

    return _shaderProgram;
}

GLuint Shader::compile_shader(GLenum _shader_type, const std::string &_shader_src)
{
    GLuint _shader = glCreateShader(_shader_type);

    const char* _src = _shader_src.c_str();
    glShaderSource(_shader, 1, &_src, nullptr);
    glCompileShader(_shader);

    GLint _success;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &_success);
    if(!_success)
    {
        char _infoLog[512];
        glGetShaderInfoLog(_shader, 512, nullptr, _infoLog);
        std::cerr << "Shader Compilation Error: " << _infoLog << std::endl;
    }

    return _shader;
}

std::ostream& operator<<(std::ostream& _os, Shader& _shader)
{
    _os << "[ Vertex shader source ]" << '\n';
    _os << _shader.m_vertex_src << '\n';
    _os << '\n';
    _os << "[ Fragment shader source ]" << '\n';
    _os << _shader.m_fragment_src << '\n';
    return _os;
}
