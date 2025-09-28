#ifndef MESH_H
#define MESH_H

enum class VertexAttribute
{
    POSITION,
    NORMAL,
    TEXCOORD_0,
    COLOR,
    TANGENT
};

struct VertexElement 
{
    VertexAttribute attribute;
    uint32_t offset;    // offset in bytes in the vertex struct
    uint32_t size;      // size in bytes of attribute (e.g. 12 for vec3 float)
    GLenum glType;      // GL_FLOAT, GL_UNSIGNED_BYTE etc.
    int components;     // number of components (e.g. 3 for vec3)
    bool normalized;    // normalized flag for GL
};

struct VertexLayout
{
    std::vector<VertexElement> elements;

    uint32_t stride;

    void addElement(VertexAttribute _attr, uint32_t _offset, uint32_t _size, GLenum _glType, int _components, bool _normalized = false)
    {
        elements.push_back({ _attr, _offset, _size, _glType, _components, _normalized });
    }
};

struct Mesh
{
    std::vector<uint8_t>      vertex_buffer;
    std::vector<unsigned int> index_buffer;

    VertexLayout layout;

    [[nodiscard]] std::size_t get_vertex_buffer_size() const { return vertex_buffer.size(); }
    [[nodiscard]] std::size_t get_index_buffer_size () const { return index_buffer .size() * sizeof(unsigned int); }
};

#endif
