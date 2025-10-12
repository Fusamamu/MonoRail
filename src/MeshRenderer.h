#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "PCH.h"
#include "Mesh.h"

class MeshRenderer
{
public:
    MeshRenderer() = default;
    ~MeshRenderer();
   
    void load_mesh      (Mesh* _mesh);
    void set_buffer_data(Mesh* _mesh);
    void set_instance_data(const std::vector<glm::mat4>& _instance_models);
    void draw() const;

    bool use_instancing = false;
    GLsizei instance_count = 1000;

private:
    GLuint m_vbo, m_vao, m_ebo, m_instance_vbo;
    
    Mesh* m_mesh;
};
#endif 
