#include "ResourceManager.h"

// Constructor / Destructor
ResourceManager::ResourceManager() = default;
ResourceManager::~ResourceManager() = default;

// Load model and store in map
bool ResourceManager::load_model(const std::string& _path, const std::string& _name)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(_path,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return false;
    }

    meshes.clear();
    process_node(scene->mRootNode, scene);

    models[_name] = meshes;
    return true;
}

Mesh ResourceManager::load_model(std::filesystem::path _path)
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
        return { };
    }

    meshes.clear();
    process_node(scene->mRootNode, scene);

    // std::string filename = _path.stem().string();
    // models[filename] = meshes;

    return meshes[0];
}

// Get meshes of a loaded model
std::vector<Mesh> ResourceManager::get_model(const std::string& name)
{
    if(models.find(name) != models.end())
        return models[name];
    return {};
}

// Process node recursively
void ResourceManager::process_node(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
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
