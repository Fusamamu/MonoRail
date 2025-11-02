#include "GizmosRenderer.h"

#include "Mesh.h"

GizmosRenderer::GizmosRenderer()
{
}

GizmosRenderer::~GizmosRenderer()
{
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);
}

void GizmosRenderer::init(GizmosType _type)
{
    Type = _type;

    switch (_type)
    {
        case GizmosType::NONE:
            break;
        case GizmosType::LINE:
            glGenVertexArrays(1, &m_vao);
            glGenBuffers     (1, &m_vbo);

            glBindVertexArray(m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

            constexpr size_t MaxLineVertices = 100000; // support 50k lines
            glBufferData(GL_ARRAY_BUFFER, MaxLineVertices * sizeof(LineVertex), nullptr, GL_DYNAMIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, color));

            glBindVertexArray(0);

            // test data
            m_line_vertices.push_back({ glm::vec3(0.0f), glm::vec3(1.0f) });
            m_line_vertices.push_back({ glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(1.0f) });
            break;
    }
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

void GizmosRenderer::update_line_vertice(const std::vector<glm::vec3>& _line_vertices)
{
    m_line_vertices.clear();
    m_line_vertices.resize(_line_vertices.size());

    for (size_t i = 0; i < _line_vertices.size(); i++)
    {
        m_line_vertices[i].position = _line_vertices[i] + glm::vec3(0.0f, 0.5f, 0.0f);
        m_line_vertices[i].color    = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_line_vertices.size() * sizeof(LineVertex), m_line_vertices.data());
    glBindVertexArray(0);
}

void GizmosRenderer::draw() const
{
    switch (Type)
    {
        case GizmosType::NONE:
            glBindVertexArray(m_vao);
            glDrawArrays(GL_POINTS, 0, 1);    // Draw one point (one AABB)
            glBindVertexArray(0);
            break;
        case GizmosType::LINE:
            glBindVertexArray(m_vao);
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, m_line_vertices.size() * sizeof(LineVertex), m_line_vertices.data());
            glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)m_line_vertices.size());
            glBindVertexArray(0);
            break;
        default:
            break;
    }
}


