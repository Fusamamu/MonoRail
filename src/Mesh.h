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

    // Helper to push a float as raw bytes
    void push_float(float v)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&v);
        vertex_buffer.insert(vertex_buffer.end(), bytes, bytes + sizeof(float));
    }

    friend std::ostream& operator<<(std::ostream& os, const Mesh& mesh)
    {
        os << "Mesh:" << std::endl;
        os << "Stride: " << mesh.layout.stride << " bytes" << std::endl;

        // Print layout
        os << "Vertex layout:" << std::endl;
        for (const auto& elem : mesh.layout.elements)
        {
            os << "  Attribute: "  << static_cast<int>(elem.attribute)
               << ", Offset: "     << elem.offset
               << ", Size: "       << elem.size
               << ", Components: " << elem.components
               << ", Type: "       << elem.glType
               << ", Normalized: " << elem.normalized
               << std::endl;
        }

        // Print vertices
        os << "Vertices (" << mesh.vertex_buffer.size() / sizeof(float) << " floats):" << std::endl;
        const float* verts = reinterpret_cast<const float*>(mesh.vertex_buffer.data());
        size_t num_floats = mesh.vertex_buffer.size() / sizeof(float);

        for (size_t i = 0; i < num_floats; i += mesh.layout.stride / sizeof(float))
        {
            os << "  Vertex " << i / (mesh.layout.stride / sizeof(float)) << ": ";
            for (size_t j = 0; j < mesh.layout.stride / sizeof(float) && (i + j) < num_floats; j++)
            {
                os << std::fixed << std::setprecision(3) << verts[i + j] << " ";
            }
            os << std::endl;
        }

        // Print indices
        os << "Indices (" << mesh.index_buffer.size() << "): ";
        for (auto idx : mesh.index_buffer)
            os << idx << " ";
        os << std::endl;

        return os;
    }
};

#endif
