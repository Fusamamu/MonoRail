#include "GizmosRenderer.h"

GizmosRenderer::GizmosRenderer()
{
}

GizmosRenderer::~GizmosRenderer()
{

}

void GizmosRenderer::create_aabb_gizmos(AABB& aabb)
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers     (1, &m_vbo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(AABB), &aabb, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AABB),(void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(AABB),(void*)sizeof(glm::vec3));

    glBindVertexArray(0);
}

void GizmosRenderer::draw() const
{
    glBindVertexArray(m_vao);
    glDrawArrays(GL_POINTS, 0, 1);    // Draw one point (one AABB)
    glBindVertexArray(0);
}


