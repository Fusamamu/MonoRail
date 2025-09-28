#include "ResourceManager.h"

void ResourceManager::init()
{
    load_model ("../res/models/teapot.fbx");
    load_shader("../res/shaders/toon.glsl");
}

void ResourceManager::load_model(std::filesystem::path _path)
{
    Assimp::Importer _importer;

    const aiScene* scene = _importer.ReadFile(_path,
        aiProcess_Triangulate      |
        aiProcess_FlipUVs          |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: " << _importer.GetErrorString() << std::endl;
        return;
    }

    m_meshes.clear();
    process_node(scene->mRootNode, scene);
    m_models[_path.stem().string()] = m_meshes;
}

// Return pointer to vector<Mesh> if exists, else nullptr
const std::vector<Mesh>* ResourceManager::get_model(const std::string& name) const
{
    auto it = m_models.find(name);
    if (it != m_models.end())
        return &(it->second); // pointer to the stored vector
    return nullptr;
}

Mesh* ResourceManager::get_first_mesh(const std::string& name)
{
    auto it = m_models.find(name);
    if (it != m_models.end() && !it->second.empty())
        return &(it->second[0]); // pointer to the first element
    return nullptr;
}

void ResourceManager::process_node(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(process_mesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        process_node(node->mChildren[i], scene);
}

Mesh ResourceManager::process_mesh(aiMesh* _ai_mesh, const aiScene* _ai_scene)
{
    Mesh _outMesh;

    // Each vertex = 8 floats = 32 bytes
    _outMesh.vertex_buffer.reserve(_ai_mesh->mNumVertices * sizeof(float) * 8);

    for (unsigned int i = 0; i < _ai_mesh->mNumVertices; i++)
    {
        // Position (x, y, z)
        _outMesh.push_float(_ai_mesh->mVertices[i].x);
        _outMesh.push_float(_ai_mesh->mVertices[i].y);
        _outMesh.push_float(_ai_mesh->mVertices[i].z);

        // std::cout << _ai_mesh->mVertices[i].x << ", "
        //           << _ai_mesh->mVertices[i].y << ", "
        //           << _ai_mesh->mVertices[i].z << std::endl;

        // Normal (nx, ny, nz)
        if (_ai_mesh->HasNormals())
        {
            _outMesh.push_float(_ai_mesh->mNormals[i].x);
            _outMesh.push_float(_ai_mesh->mNormals[i].y);
            _outMesh.push_float(_ai_mesh->mNormals[i].z);
        }
        else
        {
            _outMesh.push_float(0.0f);
            _outMesh.push_float(0.0f);
            _outMesh.push_float(0.0f);
        }

        // TexCoords (u, v)
        if (_ai_mesh->mTextureCoords[0])
        {
            _outMesh.push_float(_ai_mesh->mTextureCoords[0][i].x);
            _outMesh.push_float(_ai_mesh->mTextureCoords[0][i].y);
        }
        else
        {
            _outMesh.push_float(0.0f);
            _outMesh.push_float(0.0f);
        }
    }

    // Indices
    for (unsigned int i = 0; i < _ai_mesh->mNumFaces; i++)
    {
        aiFace face = _ai_mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            _outMesh.index_buffer.push_back(face.mIndices[j]);
    }

    // Layout
    _outMesh.layout = {};
    _outMesh.layout.addElement(VertexAttribute::POSITION  , 0                , sizeof(float) * 3, GL_FLOAT, 3);
    _outMesh.layout.addElement(VertexAttribute::NORMAL    , sizeof(float) * 3, sizeof(float) * 3, GL_FLOAT, 3);
    _outMesh.layout.addElement(VertexAttribute::TEXCOORD_0, sizeof(float) * 6, sizeof(float) * 2, GL_FLOAT, 2);
    _outMesh.layout.stride = sizeof(float) * 8;

    return _outMesh;
}

void ResourceManager::load_shader(const std::filesystem::path _path)
{
    auto name = _path.stem().string();
    m_shaders[name] = std::make_unique<Shader>(_path);
}

Shader* ResourceManager::get_shader(const std::string& name)
{
    auto it = m_shaders.find(name);
    if(it != m_shaders.end())
        return it->second.get(); // raw pointer
    return nullptr;
}
