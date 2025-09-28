#include "MeshRenderer.h"

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

void MeshRenderer::draw() const
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_mesh->index_buffer.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
