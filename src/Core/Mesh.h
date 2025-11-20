#ifndef MESH_H
#define MESH_H

#include "Vertex.h"

namespace MUG
{
    struct SubMesh;

    struct MeshRawData
    {
        std::vector<Vertex_PNT> vertex_buffer;
        std::vector<uint32_t>   index_buffer;
        std::vector<SubMesh>    sub_meshes;

        friend std::ostream& operator<<(std::ostream& _os, const MeshRawData& _data);
    };

    struct Mesh
    {
        std::vector<uint8_t > vertex_buffer;
        std::vector<uint32_t> index_buffer;
        std::vector<SubMesh > sub_meshes;

        VertexLayout layout;

        [[nodiscard]] std::size_t get_vertex_buffer_size() const { return vertex_buffer.size(); }
        [[nodiscard]] std::size_t get_index_buffer_size () const { return index_buffer .size() * sizeof(unsigned int); }

        void push_float(float v);

        friend std::ostream& operator<<(std::ostream& _os, const Mesh& _mesh);
    };

    struct SubMesh
    {
        int material_index;
        uint32_t index_offset;
        uint32_t index_count;

        friend std::ostream& operator<<(std::ostream& _os, const SubMesh& _sub_mesh);
    };

    struct BoneInfo
    {
        int id;
        glm::mat4 offset_matrix;       // Bone to model space
        glm::mat4 finalTransformation; // Final matrix passed to the shader
    };

    struct SkeletonMesh
    {
        std::vector<Vertex_Bone> vertices;
        std::vector<uint32_t>    indices;

        int bone_count = 0;
        std::map<std::string, BoneInfo> bone_mapping;
    };
}

namespace MUG::Geometry::Util
{
    Mesh convert_to_mesh(const MeshRawData& _mesh_raw_data);
    void rotate_mesh(MeshRawData& mesh, const glm::vec3& axis, float angleDegrees);
    MeshRawData get_rotated_mesh(const MeshRawData& _mesh_raw_data, const glm::vec3& _axis, float _angle_degree);
}

#endif
