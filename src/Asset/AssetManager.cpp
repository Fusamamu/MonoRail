#include "AssetManager.h"
#include "Importer.h"
#include "stb_image.h"

void AssetManager::init()
{
    load_model ("res/models/teapot.fbx"                   );
    load_model ("res/models/monkey.fbx"                   );
    load_model ("res/models/tile.fbx"                     );
    load_model ("res/models/tile_pillar.fbx"              );
    load_model ("res/models/large_plane.fbx"              );
    load_model ("res/models/arrow.fbx"                    );
    load_model ("res/models/test_idle_skeleton.fbx"       );
    load_model ("res/models/grass_blade.fbx"              );
    load_model ("res/models/base.fbx"                     );
    load_model ("res/models/sphere.fbx"                   );
    load_model ("res/models/cube.fbx"                     );
    load_model ("res/models/bevel_cube.fbx"               );
    load_model ("res/models/agent.fbx"                    );
    load_model ("res/models/train.fbx"                    );
    load_model ("res/models/water_plane.fbx"              );
    load_model ("res/rails/r_0100_0010.fbx"               );
    load_model ("res/rails/r_0001_1000.fbx"               );
    load_model ("res/rails/r_0000_0001.fbx"               );
    load_model ("res/rails/r_0000_1000.fbx"               );
    load_model ("res/rails/r_0000_0100.fbx"               );
    load_model ("res/rails/r_0000_0010.fbx"               );
    load_model ("res/rails/r_0000_0011.fbx"               );
    load_model ("res/rails/r_0000_1100.fbx"               );
    load_model ("res/rails/r_0000_0101.fbx"               );
    load_model ("res/rails/r_0000_1010.fbx"               );
    load_model ("res/rails/r_0000_1110.fbx"               );
    load_model ("res/rails/r_0000_0111.fbx"               );
    load_model ("res/rails/r_0000_1101.fbx"               );
    load_model ("res/rails/r_0000_1011.fbx"               );
    load_model ("res/rails/r_0000_1111.fbx"               );

    load_shader("res/shaders/instance.glsl"               );
    load_shader("res/shaders/toon.glsl"                   );
    load_shader("res/shaders/phong.glsl"                  );
    load_shader("res/shaders/screen_quad.glsl"            );
    load_shader("res/shaders/depth_quad.glsl"             );
    load_shader("res/shaders/fog_plane.glsl"              );
    load_shader("res/shaders/aabb.glsl"                   );
    load_shader("res/shaders/skeleton.glsl"               );
    load_shader("res/shaders/ui.glsl"                     );
    load_shader("res/shaders/ui_texture.glsl"             );
    load_shader("res/shaders/ui_texture_3d.glsl"          );
    load_shader("res/shaders/ui_noise_texture.glsl"       );
    load_shader("res/shaders/text.glsl"                   );
    load_shader("res/shaders/shell.glsl"                  );
    load_shader("res/shaders/object_instance.glsl"        );
    load_shader("res/shaders/depth_of_field.glsl"         );
    load_shader("res/shaders/line.glsl"                   );
    load_shader("res/shaders/planar_projection.glsl"      );
    load_shader("res/shaders/tile.glsl"                   );
    load_shader("res/shaders/voxel_ambient_occlusion.glsl");
    load_shader("res/shaders/water.glsl"                  );
    load_shader("res/shaders/geometry.glsl"               );

    load_mesh_raw_data(0b10000000, true , false, false, true , 3, "res/tiles/c_1000_0000.fbx");
    load_mesh_raw_data(0b11000000, true , false, false, true , 3, "res/tiles/c_1100_0000.fbx");
    load_mesh_raw_data(0b11100000, true , false, false, true , 3, "res/tiles/c_1110_0000.fbx");
    load_mesh_raw_data(0b11110000, true , false, false, false, 0, "res/tiles/c_1111_0000.fbx");
    load_mesh_raw_data(0b10100000, true , false, false, true , 1, "res/tiles/c_1010_0000.fbx");
    load_mesh_raw_data(0b10001000, true ,  true, false, true , 3, "res/tiles/c_1000_1000.fbx");
    load_mesh_raw_data(0b11001100, true ,  true, false, true , 3, "res/tiles/c_1100_1100.fbx");
    load_mesh_raw_data(0b11001000, true ,  true, true , true , 3, "res/tiles/c_1100_1000.fbx");
    load_mesh_raw_data(0b11101100, true ,  true, true , true , 3, "res/tiles/c_1110_1100.fbx");
    load_mesh_raw_data(0b11111000, false,  true, false, true , 3, "res/tiles/c_1111_1000.fbx");
    load_mesh_raw_data(0b11111100, false,  true, false, true , 3, "res/tiles/c_1111_1100.fbx");
    load_mesh_raw_data(0b11111110, false,  true, false, true , 3, "res/tiles/c_1111_1110.fbx");
    load_mesh_raw_data(0b11111010, false,  true, false, true , 1, "res/tiles/c_1111_1010.fbx");
    load_mesh_raw_data(0b11100100, true ,  true, false, true , 3, "res/tiles/c_1110_0100.fbx");
    load_mesh_raw_data(0b11101000, true ,  true, true , true , 3, "res/tiles/c_1110_1000.fbx");
    load_mesh_raw_data(0b11101110, true ,  true, false, true , 3, "res/tiles/c_1110_1110.fbx");
    load_mesh_raw_data(0b10101010, true ,  true, false, true , 1, "res/tiles/c_1010_1010.fbx");
    load_mesh_raw_data(0b10101000, true ,  true, false, true , 3, "res/tiles/c_1010_1000.fbx");
    load_mesh_raw_data(0b11101010, true ,  true, false, true , 3, "res/tiles/c_1110_1010.fbx");
}

uint8_t flip_2bit_groups(uint8_t nibble)
{
    // keep only the lowest 4 bits
    nibble &= 0x0F;

    // extract two 2-bit groups
    uint8_t high = (nibble >> 2) & 0x03; // upper 2 bits
    uint8_t low  = nibble & 0x03;        // lower 2 bits

    // reverse each 2-bit group
    auto reverse2 = [](uint8_t x) -> uint8_t {
        return ((x & 0x2) >> 1) | ((x & 0x1) << 1);
    };

    high = reverse2(high);
    low  = reverse2(low);

    // combine back
    return (high << 2) | low;
}

void AssetManager::load_mesh_raw_data(uint8_t _bit, bool _shift_high_bit, bool _shift_low_bit, bool _flip, bool _rotate, uint8_t _rotate_times, std::filesystem::path _path)
{
    MUG::MeshRawData _mesh_raw_data;

    ASSET::load_mesh_raw_data(_path, _mesh_raw_data);

    MUG::Mesh _mesh = MUG::Geometry::Util::convert_to_mesh(_mesh_raw_data);
    std::vector<MUG::Mesh> _meshes;
    _meshes.push_back(_mesh);

    uint8_t     _start_bit   = _bit;
    std::string _format_name = to_formatted_name(_start_bit);
    m_mesh_map[_format_name] = _meshes;

    if (!_rotate)
        return;

    for (size_t i = 0; i < _rotate_times; i++)
    {
        float angle = 90.0f * static_cast<float>(i + 1);

        MUG::MeshRawData _rotated_mesh_raw_data = MUG::Geometry::Util::get_rotated_mesh(_mesh_raw_data, glm::vec3(0.0f, 1.0f, 0.0f), angle);

        MUG::Mesh _mesh = MUG::Geometry::Util::convert_to_mesh(_rotated_mesh_raw_data);
        std::vector<MUG::Mesh> _meshes;
        _meshes.push_back(_mesh);

        uint8_t high  = (_start_bit >> 4) & 0x0F;
        uint8_t low   = _start_bit & 0x0F;

        if (_shift_high_bit)
            high = ((high >> 1) | ((high & 1) << 3)) & 0x0F;
        if (_shift_low_bit)
            low  = ((low >> 1)  | ((low & 1) << 3)) & 0x0F;

        _start_bit = (high << 4) | low;

        _format_name = to_formatted_name(_start_bit);

        m_mesh_map[_format_name] = _meshes;

        std::cout << _format_name << std::endl;
    }

    if (!_flip)
        return;

    MUG::MeshRawData _flipped_mesh_raw_data = MUG::Geometry::Util::get_flipped_x_mesh(_mesh_raw_data);

    _mesh = MUG::Geometry::Util::convert_to_mesh(_flipped_mesh_raw_data);
    _meshes.clear();
    _meshes.push_back(_mesh);

    _start_bit   = _bit;

    uint8_t high  = (_start_bit >> 4) & 0x0F;
    uint8_t low   = _start_bit & 0x0F;

    high = flip_2bit_groups(high);
    low  = flip_2bit_groups(low);

    _start_bit = (high << 4) | low;

    _format_name = to_formatted_name(_start_bit);
    m_mesh_map[_format_name] = _meshes;

    for (size_t i = 0; i < _rotate_times; i++)
    {
        float angle = 90.0f * static_cast<float>(i + 1);

        MUG::MeshRawData _rotated_mesh_raw_data = MUG::Geometry::Util::get_rotated_mesh(_flipped_mesh_raw_data, glm::vec3(0.0f, 1.0f, 0.0f), angle);

        MUG::Mesh _mesh = MUG::Geometry::Util::convert_to_mesh(_rotated_mesh_raw_data);
        std::vector<MUG::Mesh> _meshes;
        _meshes.push_back(_mesh);

        uint8_t high  = (_start_bit >> 4) & 0x0F;
        uint8_t low   = _start_bit & 0x0F;

        if (_shift_high_bit)
            high = ((high >> 1) | ((high & 1) << 3)) & 0x0F;
        if (_shift_low_bit)
            low  = ((low >> 1)  | ((low & 1) << 3)) & 0x0F;

        _start_bit = (high << 4) | low;

        _format_name = to_formatted_name(_start_bit);

        m_mesh_map[_format_name] = _meshes;

        std::cout << _format_name << std::endl;
    }
}

const std::vector<MUG::Mesh>* AssetManager::get_model(const std::string& name) const
{
    auto it = m_mesh_map.find(name);
    if (it != m_mesh_map.end())
        return &(it->second);
    return nullptr;
}

MUG::Mesh* AssetManager::get_first_mesh(const std::string& name)
{
    auto it = m_mesh_map.find(name);
    if (it != m_mesh_map.end() && !it->second.empty())
        return &(it->second[0]);
    return nullptr;
}

const std::vector<MUG::SkeletonMesh>* AssetManager::get_skeleton_model(const std::string& name) const
{
    auto it = m_skeleton_models.find(name);
    if (it != m_skeleton_models.end())
        return &(it->second);
    return nullptr;
}

MUG::SkeletonMesh* AssetManager::get_first_skeleton_mesh(const std::string& name)
{
    auto it = m_skeleton_models.find(name);
    if (it != m_skeleton_models.end() && !it->second.empty())
        return &(it->second[0]);
    return nullptr;
}

void AssetManager::load_texture(std::filesystem::path _path)
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

void AssetManager::load_model(std::filesystem::path _path)
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
        m_mesh_map[_path.stem().string()] = m_meshes;
    }
}

void AssetManager::process_skeleton_node(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_skeleton_meshes.push_back(process_skeleton_mesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
        process_skeleton_node(node->mChildren[i], scene);
}

MUG::SkeletonMesh AssetManager::process_skeleton_mesh(aiMesh* _ai_mesh, const aiScene* _ai_scene)
{
    MUG::SkeletonMesh _skeleton_mesh;

    size_t baseVertIndex = _skeleton_mesh.vertices.size();

    // vertices
    for (unsigned int _i = 0; _i < _ai_mesh->mNumVertices; _i++)
    {
        Vertex_Bone _vertex;
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

            MUG::BoneInfo _bone_info;
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

void AssetManager::process_node(aiNode* _node, const aiScene* _scene)
{
    for (unsigned int i = 0; i < _node->mNumMeshes; i++)
    {
        aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
        m_meshes.push_back(process_mesh(mesh, _scene));
    }

    for (unsigned int i = 0; i < _node->mNumChildren; i++)
        process_node(_node->mChildren[i], _scene);
}

MUG::Mesh AssetManager::process_mesh(aiMesh* _ai_mesh, const aiScene* _ai_scene)
{
    MUG::Mesh _outMesh;

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

void AssetManager::load_model_raw(std::filesystem::path _path)
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

    std::cout << "Loading mesh..." << _path.stem().string() << std::endl;
    m_meshes_raw_data.clear();
    process_node_raw(_scene->mRootNode, _scene);
    //m_models[_path.stem().string()] = m_meshes_raw_data;
}

void AssetManager::process_node_raw(aiNode* _node    , const aiScene* _scene)
{
    for (unsigned int i = 0; i < _node->mNumMeshes; i++)
    {
        aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
        m_meshes_raw_data.push_back(process_mesh_raw(mesh, _scene));
    }

    for (unsigned int i = 0; i < _node->mNumChildren; i++)
        process_node_raw(_node->mChildren[i], _scene);
}

MUG::MeshRawData AssetManager::process_mesh_raw(aiMesh* _ai_mesh, const aiScene* _ai_scene)
{
    MUG::MeshRawData meshData;

    meshData.vertex_buffer.reserve(_ai_mesh->mNumVertices);
    meshData.index_buffer.reserve(_ai_mesh->mNumFaces * 3); // assuming triangles

    for (unsigned int i = 0; i < _ai_mesh->mNumVertices; ++i)
    {
        Vertex_PNT vertex;

        vertex.position = glm::vec3(
            _ai_mesh->mVertices[i].x,
            _ai_mesh->mVertices[i].y,
            _ai_mesh->mVertices[i].z
        );

        if (_ai_mesh->HasNormals())
        {
            vertex.normal = glm::vec3(
                _ai_mesh->mNormals[i].x,
                _ai_mesh->mNormals[i].y,
                _ai_mesh->mNormals[i].z
            );
        }
        else
        {
            vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }

        if (_ai_mesh->HasTextureCoords(0))
        {
            vertex.texCoords = glm::vec2(
                _ai_mesh->mTextureCoords[0][i].x,
                _ai_mesh->mTextureCoords[0][i].y
            );
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        meshData.vertex_buffer.push_back(vertex);
    }

    for (unsigned int i = 0; i < _ai_mesh->mNumFaces; ++i)
    {
        const aiFace& face = _ai_mesh->mFaces[i];

        if (face.mNumIndices == 3)
        {
            meshData.index_buffer.push_back(face.mIndices[0]);
            meshData.index_buffer.push_back(face.mIndices[1]);
            meshData.index_buffer.push_back(face.mIndices[2]);
        }
    }

    return meshData;
}

void AssetManager::load_shader(const std::filesystem::path _path)
{
    auto name = _path.stem().string();
    m_shaders[name] = std::make_unique<Shader>(_path);
}

Shader* AssetManager::get_shader(const std::string& name)
{
    auto it = m_shaders.find(name);
    if(it != m_shaders.end())
        return it->second.get(); // raw pointer
    return nullptr;
}
