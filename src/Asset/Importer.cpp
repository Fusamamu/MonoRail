#include "Importer.h"

namespace ASSET
{
    MeshRawData load_mesh_raw_data(const std::filesystem::path& path, MeshRawData& _mesh_data)
    {
        Assimp::Importer _importer;

        const aiScene* _ai_scene = _importer.ReadFile(
            path,
            aiProcess_Triangulate           |
            aiProcess_GenSmoothNormals      |
            aiProcess_CalcTangentSpace      |
            aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality  |
            aiProcess_SortByPType
        );

        if (!_ai_scene || !_ai_scene->HasMeshes())
        {
            std::cerr << "Failed to load mesh: " << path << "\n";
            if (_ai_scene == nullptr)
                std::cerr << _importer.GetErrorString() << std::endl;
            return _mesh_data;
        }

        uint32_t _global_vertex_offset = 0;
        uint32_t _global_index_offset  = 0;

        for (unsigned int m = 0; m < _ai_scene->mNumMeshes; ++m)
        {
            const aiMesh* _ai_mesh = _ai_scene->mMeshes[m];

            Submesh _sub_mesh{};
            _sub_mesh.index_offset    = _global_index_offset;
            _sub_mesh.material_index = _ai_mesh->mMaterialIndex;

            for (unsigned int v = 0; v < _ai_mesh->mNumVertices; ++v)
            {
                Vertex_PNT vertex{};

                vertex.position = glm::vec3(
                    _ai_mesh->mVertices[v].x,
                    _ai_mesh->mVertices[v].y,
                    _ai_mesh->mVertices[v].z
                );

                if (_ai_mesh->HasNormals())
                {
                    vertex.normal = glm::vec3(
                        _ai_mesh->mNormals[v].x,
                        _ai_mesh->mNormals[v].y,
                        _ai_mesh->mNormals[v].z
                    );
                }

                if (_ai_mesh->HasTextureCoords(0))
                {
                    vertex.texCoords = glm::vec2(
                        _ai_mesh->mTextureCoords[0][v].x,
                        _ai_mesh->mTextureCoords[0][v].y
                    );
                }
                else
                {
                    vertex.texCoords = glm::vec2(0.0f);
                }

                _mesh_data.vertex_buffer.push_back(vertex);
            }

            for (unsigned int f = 0; f < _ai_mesh->mNumFaces; ++f)
            {
                const aiFace& face = _ai_mesh->mFaces[f];
                for (unsigned int i = 0; i < face.mNumIndices; ++i)
                    _mesh_data.index_buffer.push_back(face.mIndices[i] + _global_vertex_offset);
            }

            _sub_mesh.index_count = _ai_mesh->mNumFaces * 3; // all faces are triangles
            _mesh_data.sub_meshes.push_back(_sub_mesh);

            _global_vertex_offset += _ai_mesh->mNumVertices;
            _global_index_offset  += _sub_mesh.index_count;
        }

        return _mesh_data;
    }
}

