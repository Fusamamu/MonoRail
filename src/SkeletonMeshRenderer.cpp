#include "SkeletonMeshRenderer.h"

SkeletonMeshRenderer::SkeletonMeshRenderer()
{

}

SkeletonMeshRenderer::~SkeletonMeshRenderer()
{

}

void SkeletonMeshRenderer::load_mesh(SkeletonMesh* _mesh)
{
    m_skeleton_mesh = _mesh;
}

void SkeletonMeshRenderer::set_buffer_data(SkeletonMesh* _mesh)
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, _mesh->vertices.size() * sizeof(Vertex_Bone), _mesh->vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _mesh->indices.size() * sizeof(unsigned int), _mesh->indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Bone), (void*)offsetof(Vertex_Bone, Position ));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Bone), (void*)offsetof(Vertex_Bone, Normal   ));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_Bone), (void*)offsetof(Vertex_Bone, TexCoords));
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex_Bone), (void*)offsetof(Vertex_Bone, BoneIDs));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex_Bone), (void*)offsetof(Vertex_Bone, Weights));

    glBindVertexArray(0);
}

void SkeletonMeshRenderer::draw() const
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_skeleton_mesh->indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}