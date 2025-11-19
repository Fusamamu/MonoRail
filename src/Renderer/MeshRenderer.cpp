#include "MeshRenderer.h"
#include "Core/Mesh.h"

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

void MeshRenderer::set_instance_data(const std::vector<InstanceData>& _data)
{
    if (!m_instance_vbo)
        glGenBuffers(1, &m_instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, _data.size() * sizeof(InstanceData), _data.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(m_vao);

    std::size_t vec4Size = sizeof(glm::vec4);

    // Model matrix (locations 3–6)
    for (int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer    (3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(i * vec4Size));
        glVertexAttribDivisor    (3 + i, 1);
    }

    // Color (location 7)
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(glm::mat4)));
    glVertexAttribDivisor(7, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void MeshRenderer::update_instance_color(std::vector<InstanceData>& _data, size_t _index, const glm::vec4& _color)
{
    size_t offset = _index * sizeof(InstanceData) + sizeof(glm::mat4);
    glBindBuffer   (GL_ARRAY_BUFFER, m_instance_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(glm::vec4), glm::value_ptr(_color));
    glBindBuffer   (GL_ARRAY_BUFFER, 0);
}

void MeshRenderer::update_all_instance_colors( std::vector<InstanceData>& _data, const glm::vec4& color)
{
    // Update CPU-side copy
    for (auto& instance : _data)
        instance.color = color;

    // Send entire buffer to GPU
    glBindBuffer   (GL_ARRAY_BUFFER, m_instance_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, _data.size() * sizeof(InstanceData), _data.data());
    glBindBuffer   (GL_ARRAY_BUFFER, 0);
}

void MeshRenderer::draw() const
{
    if (!use_instancing)
    {
        if (m_mesh->sub_meshes.size() > 1)
        {
            glBindVertexArray(m_vao);
            glDrawElements(
                GL_TRIANGLES,
                (GLsizei)m_mesh->sub_meshes[0].index_count,
                GL_UNSIGNED_INT,
                reinterpret_cast<void*>(m_mesh->sub_meshes[0].index_offset * sizeof(uint32_t)));
            glBindVertexArray(0);
            return;
        }

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

void MeshRenderer::draw_mesh() const
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)m_mesh->index_buffer.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
