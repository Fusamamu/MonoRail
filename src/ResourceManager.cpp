#include "ResourceManager.h"

void ResourceManager::init()
{
    load_model ("../res/models/teapot.fbx"            );
    load_model ("../res/models/monkey.fbx"            );
    load_model ("../res/models/tile.fbx"              );
    load_model ("../res/models/tile_pillar.fbx"       );
    load_model ("../res/models/large_plane.fbx"       );
    load_model ("../res/models/arrow.fbx"             );
    load_model ("../res/models/test_idle_skeleton.fbx");
    load_model ("../res/models/grass_blade.fbx"       );
    load_model ("../res/models/base.fbx"              );
    load_model ("../res/models/sphere.fbx"            );
    load_model ("../res/models/cube.fbx"              );
    load_model ("../res/models/bevel_cube.fbx"        );

    load_shader("../res/shaders/instance.glsl"        );
    load_shader("../res/shaders/toon.glsl"            );
    load_shader("../res/shaders/phong.glsl"           );
    load_shader("../res/shaders/screen_quad.glsl"     );
    load_shader("../res/shaders/depth_quad.glsl"      );
    load_shader("../res/shaders/fog_plane.glsl"       );
    load_shader("../res/shaders/aabb.glsl"            );
    load_shader("../res/shaders/skeleton.glsl"        );
    load_shader("../res/shaders/ui.glsl"              );
    load_shader("../res/shaders/text.glsl"            );
    load_shader("../res/shaders/shell.glsl"           );
    load_shader("../res/shaders/object_instance.glsl" );
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

const std::vector<SkeletonMesh>* ResourceManager::get_skeleton_model(const std::string& name) const
{
    auto it = m_skeleton_models.find(name);
    if (it != m_skeleton_models.end())
        return &(it->second); // pointer to the stored vector
    return nullptr;
}

SkeletonMesh* ResourceManager::get_first_skeleton_mesh(const std::string& name)
{
    auto it = m_skeleton_models.find(name);
    if (it != m_skeleton_models.end() && !it->second.empty())
        return &(it->second[0]); // pointer to the first element
    return nullptr;
}

void ResourceManager::load_texture(std::filesystem::path _path)
{
    std::string _name   = _path.stem().string();
    std::string _fpStr  = _path.string();
    const char* _fpCStr = _fpStr.c_str();

    if (m_textures.contains(_name))
        return;

    std::unique_ptr<Texture> p_texture = std::make_unique<Texture>();

    auto& _texture = *p_texture;

    _texture.p_data = stbi_load(_fpCStr, &_texture.width, &_texture.height, &_texture.nrComponents, 0);
    if(!_texture.p_data)
    {
        std::cerr << "Image failed to load at path: " << _path << std::endl;
        stbi_image_free(_texture.p_data);
    }

    assert(_texture.p_data != nullptr && "No data loaded!");

    GLenum _format;

    switch(_texture.nrComponents)
    {
        case 1: _format = GL_RED ; break;
        case 3: _format = GL_RGB ; break;
        case 4: _format = GL_RGBA; break;
        default:
            std::cerr << "Unsupported image format: " << _texture.nrComponents << " channels\n";
            stbi_image_free(_texture.p_data);
            assert(false && "Unsupported image format");
    }

    glGenTextures   (1, &_texture.texture_id);
    glBindTexture   (GL_TEXTURE_2D, _texture.texture_id);
    glTexImage2D    (GL_TEXTURE_2D, 0, _format, _texture.width, _texture.height, 0, _format, GL_UNSIGNED_BYTE, _texture.p_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture   (GL_TEXTURE_2D, 0);

    stbi_image_free(_texture.p_data);

    m_textures[_name] = std::move(p_texture);
}

void ResourceManager::load_model(std::filesystem::path _path)
{
    Assimp::Importer _importer;

    const aiScene* _scene = _importer.ReadFile(_path,
        aiProcess_Triangulate      |
        aiProcess_FlipUVs          |
        aiProcess_CalcTangentSpace |
        aiProcess_GenNormals);

    if (!_scene || _scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !_scene->mRootNode)
    {
        std::cerr << "ERROR::ASSIMP:: " << _importer.GetErrorString() << std::endl;
        return;
    }

    if (_scene->HasAnimations())
    {
        std::cout << "Loading skeletons..." << _path.stem().string() << std::endl;
        m_skeleton_meshes.clear();
        process_skeleton_node(_scene->mRootNode, _scene);
        m_skeleton_models[_path.stem().string()] = m_skeleton_meshes;
    }
    else
    {
        std::cout << "Loading mesh..." << _path.stem().string() << std::endl;
        m_meshes.clear();
        process_node(_scene->mRootNode, _scene);
        m_models[_path.stem().string()] = m_meshes;
    }
}

void ResourceManager::process_skeleton_node(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_skeleton_meshes.push_back(process_skeleton_mesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        process_skeleton_node(node->mChildren[i], scene);
}

SkeletonMesh ResourceManager::process_skeleton_mesh(aiMesh* _ai_mesh, const aiScene* _ai_scene)
{
    SkeletonMesh _skeleton_mesh;

    size_t baseVertIndex = _skeleton_mesh.vertices.size();

    // vertices
    for (unsigned int _i = 0; _i < _ai_mesh->mNumVertices; _i++)
    {
        Vertex _vertex;
        _vertex.Position  = glm::vec3(_ai_mesh->mVertices[_i].x, _ai_mesh->mVertices[_i].y, _ai_mesh->mVertices[_i].z);
        _vertex.Normal    = glm::vec3(_ai_mesh->mNormals [_i].x, _ai_mesh->mNormals [_i].y, _ai_mesh->mNormals [_i].z);
        _vertex.TexCoords = glm::vec2(_ai_mesh->mTextureCoords[0][_i].x, _ai_mesh->mTextureCoords[0][_i].y);

        _skeleton_mesh.vertices.push_back(_vertex);
    }

    // indices
    for (unsigned int _i = 0; _i < _ai_mesh->mNumFaces; _i++)
    {
        aiFace _face = _ai_mesh->mFaces[_i];
        for (unsigned int j = 0; j < _face.mNumIndices; j++)
            _skeleton_mesh.indices.push_back(baseVertIndex + _face.mIndices[j]);
    }

    // bones & weights
    for (unsigned int i = 0; i < _ai_mesh->mNumBones; i++)
    {
        aiBone* _ai_bone = _ai_mesh->mBones[i];
        std::string _bone_name = _ai_bone->mName.C_Str();

        int _bone_index = -1;

        if (_skeleton_mesh.bone_mapping.find(_bone_name) == _skeleton_mesh.bone_mapping.end())
        {
            _bone_index = _skeleton_mesh.bone_count;
            _skeleton_mesh.bone_count++;

            BoneInfo _bone_info;
            _bone_info.id = _bone_index;
            _bone_info.offset_matrix       = glm::transpose(glm::make_mat4(&_ai_bone->mOffsetMatrix.a1));
            _bone_info.finalTransformation = glm::mat4(1.0f);

            _skeleton_mesh.bone_mapping[_bone_name] = _bone_info;

        }
        else
        {
            _bone_index = _skeleton_mesh.bone_mapping[_bone_name].id;
        }

        for (unsigned int _w = 0; _w < _ai_bone->mNumWeights; _w++)
        {
            unsigned int vertexID = _ai_bone->mWeights[_w].mVertexId;
            float weight          = _ai_bone->mWeights[_w].mWeight;

            _skeleton_mesh.vertices[baseVertIndex + vertexID].add_bone_data(_bone_index, weight);
        }
    }

    return _skeleton_mesh;
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
    _outMesh.layout.add_element(VertexAttribute::POSITION  , 0                , sizeof(float) * 3, GL_FLOAT, 3);
    _outMesh.layout.add_element(VertexAttribute::NORMAL    , sizeof(float) * 3, sizeof(float) * 3, GL_FLOAT, 3);
    _outMesh.layout.add_element(VertexAttribute::TEXCOORD_0, sizeof(float) * 6, sizeof(float) * 2, GL_FLOAT, 2);
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
