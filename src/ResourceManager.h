#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "PCH.h"
#include "Mesh.h"
#include "Shader.h"
#include "stb_image.h"

struct Texture
{
    GLuint texture_id;
    GLenum texture_target = GL_TEXTURE_2D;
    int width, height, nrComponents;
    unsigned char* p_data;

    void bind(int _slot = 0) const
    {
        glActiveTexture(GL_TEXTURE0 + _slot);
        glBindTexture(texture_target, texture_id);
    }
};

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

    void load_texture(std::filesystem::path _path);

    const std::vector<Mesh>* get_model(const std::string& name) const;
    Mesh* get_first_mesh(const std::string& name);

    const std::vector<SkeletonMesh>* get_skeleton_model(const std::string& name) const;
    SkeletonMesh* get_first_skeleton_mesh(const std::string& name);

    void load_shader(const std::filesystem::path _path);
    Shader* get_shader(const std::string& _name);

private:
    ResourceManager() = default;   // private constructor
    ~ResourceManager() = default;

    std::vector<Mesh>                              m_meshes;
    std::vector<SkeletonMesh>                      m_skeleton_meshes;

    std::map<std::string, std::vector<Mesh>>          m_models;
    std::map<std::string, std::vector<SkeletonMesh>>  m_skeleton_models;
    std::map<std::string, std::unique_ptr<Shader>>    m_shaders;
    std::map<std::string, std::unique_ptr<Texture>>   m_textures;

    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* _ai_mesh, const aiScene* _ai_scene);

    void process_skeleton_node(aiNode* node, const aiScene* scene);
    SkeletonMesh process_skeleton_mesh(aiMesh* _ai_mesh, const aiScene* _ai_scene);
};

#endif
