#include "Mesh.h"

namespace MUG
{
    void Mesh::push_float(float v)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&v);
        vertex_buffer.insert(vertex_buffer.end(), bytes, bytes + sizeof(float));
    }
}

std::ostream& MUG::operator<<(std::ostream& _os, const MUG::MeshRawData& _data)
{
    for (const MUG::SubMesh& _sub_mesh : _data.sub_meshes)
        _os << _sub_mesh << "\n";
    return _os;
}

std::ostream& MUG::operator<<(std::ostream& _os, const MUG::Mesh& _mesh)
{
    _os << "Mesh:" << std::endl;
    _os << "Stride: " << _mesh.layout.stride << " bytes" << std::endl;

    _os << "Vertex layout:" << std::endl;
    for (const auto& elem : _mesh.layout.elements)
    {
        _os << "  Attribute: "  << static_cast<int>(elem.attribute)
           << ", Offset: "     << elem.offset
           << ", Size: "       << elem.size
           << ", Components: " << elem.components
           << ", Type: "       << elem.glType
           << ", Normalized: " << elem.normalized
           << std::endl;
    }

    _os << "Vertices (" << _mesh.vertex_buffer.size() / sizeof(float) << " floats):" << std::endl;
    const float* verts = reinterpret_cast<const float*>(_mesh.vertex_buffer.data());
    size_t num_floats = _mesh.vertex_buffer.size() / sizeof(float);

    for (size_t i = 0; i < num_floats; i += _mesh.layout.stride / sizeof(float))
    {
        _os << "  Vertex " << i / (_mesh.layout.stride / sizeof(float)) << ": ";
        for (size_t j = 0; j < _mesh.layout.stride / sizeof(float) && (i + j) < num_floats; j++)
        {
            _os << std::fixed << std::setprecision(3) << verts[i + j] << " ";
        }
        _os << std::endl;
    }

    _os << "Indices (" << _mesh.index_buffer.size() << "): ";
    for (auto idx : _mesh.index_buffer)
        _os << idx << " ";
    _os << std::endl;

    return _os;
}

std::ostream& MUG::operator<<(std::ostream& _os, const MUG::SubMesh& _sub_mesh)
{
    _os << "Material index: " << _sub_mesh.material_index << "\n"
        << "Index offset: "   << _sub_mesh.index_offset   << "\n"
        << "Index count: "    << _sub_mesh.index_count;
    return _os;
}

namespace MUG::Geometry::Util
{
    Mesh convert_to_mesh(const MeshRawData& _mesh_raw_data)
    {
        Mesh _mesh;

        _mesh.vertex_buffer.resize(_mesh_raw_data.vertex_buffer.size() * sizeof(Vertex_PNT));
        std::memcpy(_mesh.vertex_buffer.data(), _mesh_raw_data.vertex_buffer.data(), _mesh.vertex_buffer.size());
        _mesh.index_buffer = _mesh_raw_data.index_buffer;
        _mesh.sub_meshes   = _mesh_raw_data.sub_meshes;

        _mesh.layout = {};
        _mesh.layout.add_element(VertexAttribute::POSITION  , 0                , sizeof(float) * 3, GL_FLOAT, 3);
        _mesh.layout.add_element(VertexAttribute::NORMAL    , sizeof(float) * 3, sizeof(float) * 3, GL_FLOAT, 3);
        _mesh.layout.add_element(VertexAttribute::TEXCOORD_0, sizeof(float) * 6, sizeof(float) * 2, GL_FLOAT, 2);
        _mesh.layout.stride = sizeof(float) * 8;

        return _mesh;
    }

    void rotate_mesh(MeshRawData& mesh, const glm::vec3& axis, float angleDegrees)
    {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), glm::normalize(axis));

        for (auto& vertex : mesh.vertex_buffer)
        {
            glm::vec4 pos = rotation * glm::vec4(vertex.position, 1.0f);
            vertex.position = glm::vec3(pos);

            glm::vec4 nrm = rotation * glm::vec4(vertex.normal, 0.0f);
            vertex.normal = glm::normalize(glm::vec3(nrm));
        }
    }

    MeshRawData get_rotated_mesh(const MeshRawData& src, const glm::vec3& axis, float angleDegrees)
    {
        MeshRawData dst;

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), glm::normalize(axis));

        dst.vertex_buffer.reserve(src.vertex_buffer.size());
        dst.index_buffer  = src.index_buffer;
        dst.sub_meshes    = src.sub_meshes;

        for (const auto& vertex : src.vertex_buffer)
        {
            Vertex_PNT v;
            v.position  = glm::vec3(rotation * glm::vec4(vertex.position, 1.0f));
            v.normal    = glm::vec3(rotation * glm::vec4(vertex.normal, 0.0f));
            v.texCoords = vertex.texCoords;

            dst.vertex_buffer.push_back(v);
        }

        return dst;
    }

}
