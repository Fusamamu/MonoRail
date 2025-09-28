#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "PCH.h"
#include "Mesh.h"
#include "Shader.h"

class ResourceManager
{
public:
    ResourceManager(const ResourceManager&)            = delete;
    ResourceManager(ResourceManager&&)                 = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&)      = delete;

    static ResourceManager& instance()
    {
        static ResourceManager _instance;
        return _instance;
    }

    void init();
    void load_model(std::filesystem::path _path);

    const std::vector<Mesh>* get_model(const std::string& name) const;
    Mesh* get_first_mesh(const std::string& name);

    void load_shader(const std::filesystem::path _path);
    Shader* get_shader(const std::string& _name);

private:
    ResourceManager() = default;   // private constructor
    ~ResourceManager() = default;

    std::vector<Mesh>                              m_meshes;
    std::map<std::string, std::vector<Mesh>>       m_models;
    std::map<std::string, std::unique_ptr<Shader>> m_shaders;

    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* _ai_mesh, const aiScene* _ai_scene);
};

#endif
