#ifndef MESH_H
#define MESH_H

#include "../PCH.h"
#include "Vertex.h"

struct Submesh {
    uint32_t indexOffset;
    uint32_t indexCount;
    int material_index;
};

struct MeshRawData
{
    std::vector<Vertex_PNT> vertex_buffer;
    std::vector<uint32_t>   index_buffer;
    std::vector<Submesh>    sub_meshes;
};

struct Mesh
{
    std::vector<uint8_t>  vertex_buffer;
    std::vector<uint32_t> index_buffer;

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

        os << "Indices (" << mesh.index_buffer.size() << "): ";
        for (auto idx : mesh.index_buffer)
            os << idx << " ";
        os << std::endl;

        return os;
    }
};

struct BoneInfo
{
    int id;
    glm::mat4 offset_matrix;       // Bone to model space
    glm::mat4 finalTransformation; // Final matrix passed to the shader
};

struct SkeletonMesh
{
    std::vector<Vertex_Bone>   vertices;
    std::vector<uint32_t> indices;

    int bone_count = 0;
    std::map<std::string, BoneInfo> bone_mapping;
};

inline Mesh convert_to_mesh(const MeshRawData& _mesh_raw_data)
{
    Mesh _mesh;

    _mesh.vertex_buffer.resize(_mesh_raw_data.vertex_buffer.size() * sizeof(Vertex_PNT));
    std::memcpy(_mesh.vertex_buffer.data(), _mesh_raw_data.vertex_buffer.data(), _mesh.vertex_buffer.size());
    _mesh.index_buffer = _mesh_raw_data.index_buffer;

    _mesh.layout = {};
    _mesh.layout.add_element(VertexAttribute::POSITION  , 0                , sizeof(float) * 3, GL_FLOAT, 3);
    _mesh.layout.add_element(VertexAttribute::NORMAL    , sizeof(float) * 3, sizeof(float) * 3, GL_FLOAT, 3);
    _mesh.layout.add_element(VertexAttribute::TEXCOORD_0, sizeof(float) * 6, sizeof(float) * 2, GL_FLOAT, 2);
    _mesh.layout.stride = sizeof(float) * 8;

    return _mesh;
}

namespace Geometry::Util
{
    void rotate_mesh(MeshRawData& mesh, const glm::vec3& axis, float angleDegrees);
    MeshRawData get_rotated_mesh(const MeshRawData& _mesh_raw_data, const glm::vec3& _axis, float _angle_degree);
}

#endif
