#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "PCH.h"
#include "Mesh.h"

class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    bool load_model(const std::string& _path, const std::string& _name);
    Mesh load_model(std::filesystem::path _path);
    std::vector<Mesh> get_model(const std::string& name);

private:
    std::vector<Mesh> meshes;
    std::map<std::string, std::vector<Mesh>> models;

    void process_node(aiNode* node, const aiScene* scene);
    Mesh process_mesh(aiMesh* _ai_mesh, const aiScene* _ai_scene);
};

#endif
