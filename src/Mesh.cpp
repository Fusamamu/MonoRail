#include "Mesh.h"

namespace Geometry::Util
{
    void rotate_mesh(MeshRawData& mesh, const glm::vec3& axis, float angleDegrees)
    {
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), glm::normalize(axis));

        for (auto& vertex : mesh.vertex_buffer)
        {
            glm::vec4 pos = rotation * glm::vec4(vertex.Position, 1.0f);
            vertex.Position = glm::vec3(pos);

            glm::vec4 nrm = rotation * glm::vec4(vertex.Normal, 0.0f);
            vertex.Normal = glm::normalize(glm::vec3(nrm));
        }
    }

    MeshRawData get_rotated_mesh(const MeshRawData& mesh, const glm::vec3& axis, float angleDegrees)
    {
        MeshRawData _new_mesh;

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDegrees), glm::normalize(axis));

        for (auto& vertex : mesh.vertex_buffer)
        {
            Vertex_PNT _vertex;
            _vertex.Position  = rotation * glm::vec4(vertex.Position, 1.0f);
            _vertex.Normal    = rotation * glm::vec4(vertex.Normal, 0.0f);
            _vertex.TexCoords = vertex.TexCoords;

            _new_mesh.vertex_buffer.push_back(_vertex);
        }

        return std::move(_new_mesh);
    }
}
