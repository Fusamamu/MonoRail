#include "MeshRenderer.h"

MeshRenderer::~MeshRenderer()
{
    if (m_vbo)          glDeleteBuffers     (1, &m_vbo);
    if (m_ebo)          glDeleteBuffers     (1, &m_ebo);
    if (m_instance_vbo) glDeleteBuffers     (1, &m_instance_vbo);
    if (m_vao)          glDeleteVertexArrays(1, &m_vao);
}

void MeshRenderer::load_mesh(Mesh* _mesh)
{
    m_mesh = _mesh;
}

void MeshRenderer::set_buffer_data(Mesh* _mesh)
{
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER        , m_vbo);
    glBufferData(GL_ARRAY_BUFFER        , _mesh->get_vertex_buffer_size(), _mesh->vertex_buffer.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _mesh->get_index_buffer_size() , _mesh->index_buffer.data() , GL_STATIC_DRAW);

    uint32_t _attribIndex = 0;
    for (const auto& _element : _mesh->layout.elements)
    {
        glEnableVertexAttribArray(_attribIndex);
        glVertexAttribPointer
        (
            _attribIndex,
            _element.components,
            _element.glType,
            _element.normalized ? GL_TRUE : GL_FALSE,
            _mesh->layout.stride,
            (void*)(uintptr_t)_element.offset
        );
        _attribIndex++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Add instance buffer
void MeshRenderer::set_instance_data(const std::vector<glm::mat4>& _instance_models)
{
    if (!m_instance_vbo)
        glGenBuffers(1, &m_instance_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);

    glBufferData(GL_ARRAY_BUFFER, _instance_models.size() * sizeof(glm::mat4), _instance_models.data(), GL_STATIC_DRAW);

    std::size_t vec4Size = sizeof(glm::vec4);

    u_int32_t _meshAttribIndex = 3;

    // mat4 consumes 4 attribute slots
    for (int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(_meshAttribIndex + i); // pick the next available attribute index
        glVertexAttribPointer    (_meshAttribIndex + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
        glVertexAttribDivisor    (_meshAttribIndex + i, 1); // per-instance
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MeshRenderer::draw() const
{
    if (!use_instancing)
    {
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)m_mesh->index_buffer.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
    else
    {
        glBindVertexArray(m_vao);
        glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)m_mesh->index_buffer.size(), GL_UNSIGNED_INT, nullptr, instance_count);
        glBindVertexArray(0);
    }
}
