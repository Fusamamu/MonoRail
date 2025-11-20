#ifndef SKELETONMESHRENDERER_H
#define SKELETONMESHRENDERER_H

namespace MUG
{
    class SkeletonMesh;
}

class SkeletonMeshRenderer
{
public:
    SkeletonMeshRenderer();
    ~SkeletonMeshRenderer();

    void load_mesh      (MUG::SkeletonMesh* _mesh);
    void set_buffer_data(MUG::SkeletonMesh* _mesh);
    void draw() const;

private:
    unsigned int m_vbo, m_vao, m_ebo;
    MUG::SkeletonMesh* m_skeleton_mesh;
};

#endif
