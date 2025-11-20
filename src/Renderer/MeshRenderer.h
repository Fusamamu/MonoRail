#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include <GL/glew.h>

namespace MUG
{
    struct Mesh;
}

struct InstanceData
{
    glm::mat4 model;
    glm::vec4 color;
};

class MeshRenderer
{
public:
    MeshRenderer() = default;
    ~MeshRenderer();

    void load_mesh      (MUG::Mesh* _mesh);
    void set_buffer_data(MUG::Mesh* _mesh);
    void set_instance_data          (const std::vector<InstanceData>& _data);

    void update_instance_color      (std::vector<InstanceData>& _data, size_t _index, const glm::vec4& _color);
    void update_all_instance_colors( std::vector<InstanceData>& _data, const glm::vec4& color);

    void draw     () const;
    void draw_mesh() const;

    bool use_instancing = false;
    GLsizei instance_count = 1000;

private:
    GLuint m_vbo, m_vao, m_ebo, m_instance_vbo;
    
    MUG::Mesh* m_mesh;
};
#endif 
