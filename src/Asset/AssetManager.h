#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "Core/Mesh.h"
#include "Renderer/Shader.h"
#include "Asset/Texture.h"

inline std::string to_formatted_name(uint8_t value)
{
    std::string bits = std::bitset<8>(value).to_string();
    bits.insert(4, "_");
    return "c_" + bits;
}

class AssetManager
{
public:
    AssetManager(const AssetManager&)            = delete;
    AssetManager(AssetManager&&)                 = delete;
    AssetManager& operator=(const AssetManager&) = delete;
    AssetManager& operator=(AssetManager&&)      = delete;

    static AssetManager& instance()
    {
        static AssetManager _instance;
        return _instance;
    }

    void init();
    void load_model    (std::filesystem::path _path);
    void load_model_raw(std::filesystem::path _path);

    void load_mesh_raw_data(uint8_t _bit, bool _shift_high_bit, bool _shift_low_bit, bool _rotate, uint8_t _rotate_times, std::filesystem::path _path);

    void load_texture(std::filesystem::path _path);

    const std::vector<MUG::Mesh>* get_model(const std::string& name) const;
    MUG::Mesh* get_first_mesh(const std::string& name);

    const std::vector<MUG::SkeletonMesh>* get_skeleton_model(const std::string& name) const;
    MUG::SkeletonMesh* get_first_skeleton_mesh(const std::string& name);

    auto shader_view()
    {
        return std::views::values(m_shaders);
    }
    void load_shader(const std::filesystem::path _path);
    Shader* get_shader(const std::string& _name);

private:
    AssetManager() = default;
    ~AssetManager() = default;

    std::vector<MUG::MeshRawData>             m_meshes_raw_data;
    std::map<std::string, MUG::MeshRawData>   m_mesh_raw_data_map;

    std::vector<MUG::Mesh>           m_meshes;
    std::vector<MUG::SkeletonMesh>   m_skeleton_meshes;

    std::map<std::string, std::vector<MUG::Mesh>>          m_mesh_map;
    std::map<std::string, std::vector<MUG::SkeletonMesh>>  m_skeleton_models;
    std::map<std::string, std::unique_ptr<Shader>>    m_shaders;
    std::map<std::string, std::unique_ptr<Texture>>   m_textures;

    void process_node                (aiNode* _node    , const aiScene* _scene);
    MUG::Mesh process_mesh           (aiMesh* _ai_mesh, const aiScene* _ai_scene);
    void process_node_raw            (aiNode* _node    , const aiScene* _scene);
    MUG::MeshRawData process_mesh_raw(aiMesh* _ai_mesh, const aiScene* _ai_scene);

    void process_skeleton_node(aiNode* node, const aiScene* scene);
    MUG::SkeletonMesh process_skeleton_mesh(aiMesh* _ai_mesh, const aiScene* _ai_scene);
};

#endif
