#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Mesh.h"

class MeshRenderer
{
public:
    MeshRenderer() = default;
    ~MeshRenderer();
   
    void load_mesh      (Mesh* _mesh);
    void set_buffer_data(Mesh* _mesh);
    void draw() const;

private:
    unsigned int m_vbo, m_vao, m_ebo;
    
    Mesh* m_mesh;
};
#endif 
