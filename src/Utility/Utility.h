#ifndef UTILITY_H
#define UTILITY_H

#include <vector>
#include <bitset>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Scene/Scene.h"
#include "Procedural/PerlinNoise.h"
#include "Components/Transform.h"
#include "Renderer/Material.h"
#include "Renderer/MeshRenderer.h"
#include "Navigation/Agent.h"

namespace Utility
{
    inline std::vector<glm::mat4> generate_random_instances(int count)
    {
        std::srand(static_cast<unsigned int>(std::time(0)));
        std::vector<glm::mat4> _instance_models;
        _instance_models.reserve(count);

        for (int i = 0; i < count; ++i)
        {
            // Random position in a 50x50 area
            float x = static_cast<float>(std::rand() % 5000) / 100.0f - 25.0f;
            float z = static_cast<float>(std::rand() % 5000) / 100.0f - 25.0f;
            float y = 0.0f; // ground level

            glm::mat4 _model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));

            // Random rotation around Y axis
            float _angle = static_cast<float>(std::rand() % 360);
            _model = glm::rotate(_model, glm::radians(_angle), glm::vec3(0, 1, 0));

            // Random uniform scale
            float _scale = 0.5f + static_cast<float>(std::rand() % 50) / 100.0f; // 0.5 - 1.0
            _model = glm::scale(_model, glm::vec3(_scale));

            _instance_models.push_back(_model);
        }

        return _instance_models;
    }

   	inline std::vector<glm::mat4> generate_grass_instances(int _patch_count, int _blades_per_patch, int _layers)
    {
        std::vector<glm::mat4> _instance_models;

        for (int p = 0; p < _patch_count; p++)
        {
            // Random patch position
            float patchX = ((rand() % 1000) / 1000.0f - 0.5f) * 50.0f;
            float patchZ = ((rand() % 1000) / 1000.0f - 0.5f) * 50.0f;

            for (int b = 0; b < _blades_per_patch; b++)
            {
                float offsetX = ((rand() % 1000) / 1000.0f - 0.5f);
                float offsetZ = ((rand() % 1000) / 1000.0f - 0.5f);

                for (int l = 0; l < _layers; l++)
                {
                    glm::mat4 _model = glm::mat4(1.0f);

                    _model = glm::translate(_model, glm::vec3(patchX + offsetX, l * 0.1f, patchZ + offsetZ));

                    float _angle = ((rand() % 1000) / 1000.0f) * 360.0f;
                    _model = glm::rotate(_model, glm::radians(_angle), glm::vec3(0,1,0));

                    float _scale = 0.8f + ((rand() % 200) / 1000.0f); // 0.8 - 1.0
                    _model = glm::scale(_model, glm::vec3(_scale, 1.0f, _scale));

                    _instance_models.push_back(_model);
                }
            }
        }
        return _instance_models;
    }

    inline void create_grass(Scene* _scene)
    {
        Material _grass_material;
        _grass_material.shader_id = "instance";

        entt::entity _grass_e      = _scene->create_object("grass" , "grass_blade" , {10.0f, 10.0f, 0.0f}  , _grass_material) ;
        auto& _grass_mesh_renderer = _scene->get_registry().get<MeshRenderer>(_grass_e);

        std::vector<glm::mat4> _instance_models = generate_random_instances(40000);

        _grass_mesh_renderer.use_instancing = true;
        _grass_mesh_renderer.instance_count = _instance_models.size();
    }

    inline void create_grass_shell(Scene* _scene)
    {
        Procgen::PerlinNoise _perlin_noise;
        GLuint _noise_id = _perlin_noise.generate_perlin_texture(512, 512, 100.0f, 1337);

        Material _shell_material;
        _shell_material.shader_id  = "shell";
        _shell_material.diffuseMap = _noise_id;

        entt::entity _shell_e      = _scene->create_object("shell" , "large_plane" , { 0.0f,  1.0f, 0.0f}  , _shell_material) ;
        auto& _shell_transform     = _scene->get_registry().get<Component::Transform>   (_shell_e);
        auto& _shell_mesh_renderer = _scene->get_registry().get<MeshRenderer>(_shell_e);

        _shell_transform.scale = glm::vec3(0.3f, 0.3f, 0.3f);

        _shell_mesh_renderer.use_instancing = true;
        _shell_mesh_renderer.instance_count = 20;
    }

    inline void create_test_objects(Scene* _scene)
    {
        Material _phong_material;
        _phong_material.shader_id    = "phong";
        _phong_material.diffuse_color = glm::vec3(1.0f, 1.0f, 1.0f);

        Material _toon_material;
        _toon_material.shader_id = "toon";

        Material _fog_plane_material;
        _fog_plane_material.shader_id = "fog_plane";
        _fog_plane_material.depth_write = false;

        _scene->create_object("object", "large_plane" , {0.0f, -2.0f, 0.0f}, _fog_plane_material);

        _scene->create_object("object", "teapot"      , {5.0f,  0.0f, 0.0f}, _phong_material);
        _scene->create_object("object", "monkey"      , {0.0f,  1.0f, 0.0f}, _toon_material) ;

        entt::entity _agent_e = _scene->create_object("player", "teapot"      , { 0.0f,  0.0f, 5.0f}  , _toon_material);
        entt::entity _child_e = _scene->create_object("child" , "teapot"      , { 0.0f,  0.0f, 8.0f}  , _toon_material);
        _scene->get_registry().emplace<NAV::Agent>   (_agent_e);
        _scene->get_registry().emplace<Parent>  (_child_e).entity = _agent_e;

        create_grass      (_scene);
        create_grass_shell(_scene);

        auto _train_e = _scene->create_object("train", "train", { 0.0f, 0.5f, 0.0f }, _phong_material);
        auto& _train_agent = _scene->get_registry().emplace<NAV::Agent>(_train_e);

        _scene->create_object("floor", "large_plane" , {0.0f, 0.52f, 0.0f}, _phong_material);
    }

    inline float randomFloat(float _min, float _max)
    {
        return _min + static_cast<float>(rand()) / RAND_MAX * (_max - _min);
    }

    inline std::string to_formatted_name(uint8_t _value)
    {
        std::string bits = std::bitset<8>(_value).to_string();
        bits.insert(4, "_");
        return "c_" + bits;
    }
}

#endif //UTILITY_H
