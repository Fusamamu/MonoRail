#ifndef SKELETONMESHRENDERER_H
#define SKELETONMESHRENDERER_H

#include "PCH.h"
#include "Mesh.h"

class SkeletonMeshRenderer
{
public:
    SkeletonMeshRenderer();
    ~SkeletonMeshRenderer();

    void load_mesh      (SkeletonMesh* _mesh);
    void set_buffer_data(SkeletonMesh* _mesh);
    void draw() const;

private:
    unsigned int m_vbo, m_vao, m_ebo;
    SkeletonMesh* m_skeleton_mesh;
};

#endif
