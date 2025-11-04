#include "Mesh.h"

namespace Geometry::Util
{
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
