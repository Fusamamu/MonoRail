#include "AgentSystem.h"

#include "Components/Component.h"
#include "Asset/AssetManager.h"
#include "Components/Transform.h"

namespace EntitySystem
{
    void AgentSystem::init(entt::registry& _registry, glm::vec2 _bounds)
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));

        const glm::vec2 areaSize(20.0f, 20.0f);

        float _padding = 1.0f;

        const size_t instanceCount = 100;

        m_agent_instances.reserve(instanceCount);

        for (size_t i = 0; i < instanceCount; ++i)
        {
            float x = ((float)std::rand() / RAND_MAX - 0.5f) * areaSize.x + 10.0f;
            float z = ((float)std::rand() / RAND_MAX - 0.5f) * areaSize.y + 10.0f;

            // Define your bounds
            float minX = 0.2f;
            float maxX = areaSize.x - _padding;
            float minZ = 0.2f;
            float maxZ = areaSize.y - _padding;

            // Clamp values
            x = std::clamp(x, minX, maxX);
            z = std::clamp(z, minZ, maxZ);

            glm::mat4 model(1.0f);
            model = glm::translate(model, glm::vec3(x, 0.5f, z));
            //model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // rotation 0 radians
            model = glm::scale (model, glm::vec3(1.0f));

            glm::vec4 color(
                (float)std::rand() / RAND_MAX,
                (float)std::rand() / RAND_MAX,
                (float)std::rand() / RAND_MAX,
                1.0f
            );

            m_agent_instances.push_back({ model, color });


            glm::vec2 dir(
                ((float)std::rand() / RAND_MAX - 0.5f) * 2.0f,
                ((float)std::rand() / RAND_MAX - 0.5f) * 2.0f
            );
            dir = glm::normalize(dir);
            m_agent_velocities.push_back(dir);
        }

        m_instance_entity = _registry.create();

        Mesh* _p_mesh = AssetManager::instance().get_first_mesh("agent");

        auto& _node = _registry.emplace<Node>(m_instance_entity, Node());
        _node.name = "agent_instances";

        auto& _transform = _registry.emplace<Component::Transform>(m_instance_entity);
        _transform.position = glm::vec3(0.0f);

        Material _material;
        _material.shader_id = "object_instance";
        _material.rgba      = glm::vec4(1.0f, 0.0f, 0.0f, 0.1f);

        auto& _material_comp = _registry.emplace<Material>(m_instance_entity);
        _material_comp.shader_id   = _material.shader_id;
        _material_comp.depth_test  = _material.depth_test;
        _material_comp.depth_write = _material.depth_write;
        _material_comp.diffuseMap  = _material.diffuseMap;
        _material_comp.rgba        = _material.rgba;
        _material_comp.cast_shadow = false;

        auto& _mesh_renderer = _registry.emplace<MeshRenderer>(m_instance_entity);
        _mesh_renderer.load_mesh      (_p_mesh);
        _mesh_renderer.set_buffer_data(_p_mesh);

        _mesh_renderer.use_instancing = true;
        _mesh_renderer.instance_count = m_agent_instances.size();
        _mesh_renderer.set_instance_data(m_agent_instances);
    }

    void AgentSystem::update(entt::registry& _registry)
    {
        const float moveSpeed   = 5.0f;  // units per second
        const float turnChance  = 0.005f; // 1% chance to change direction per frame
        const glm::vec2 bounds(20.0f, 20.0f);

        float dt = 0.0016f;

        for (size_t i = 0; i < m_agent_instances.size(); ++i)
        {
            InstanceData& instance = m_agent_instances[i];

            glm::vec2& dir = m_agent_velocities[i];

            // Occasionally change direction
            if ((float)std::rand() / RAND_MAX < turnChance)
            {
                glm::vec2 newDir(
                    ((float)std::rand() / RAND_MAX - 0.5f) * 2.0f,
                    ((float)std::rand() / RAND_MAX - 0.5f) * 2.0f
                );
                dir = glm::normalize(newDir);
            }

            // Extract current position from model matrix
            glm::vec3 pos = instance.model[3]; // column 3 = translation
            pos.x += dir.x * moveSpeed * dt;
            pos.z += dir.y * moveSpeed * dt;

            // Bounce off edges
            float halfX = bounds.x * 0.5f;
            float halfZ = bounds.y * 0.5f;

            if (pos.x < 0.0f || pos.x > bounds.x)
            {
                dir.x = -dir.x;
                pos.x = glm::clamp(pos.x, 0.0f, bounds.x);
            }

            if (pos.z < 0.0f || pos.z > bounds.y)
            {
                dir.y = -dir.y;
                pos.z = glm::clamp(pos.z, 0.0f, bounds.y);
            }

            // Compute facing angle from direction
            float angle = std::atan2(dir.x, dir.y);

            instance.model = glm::mat4(1.0f);
            instance.model = glm::translate(instance.model, glm::vec3(pos.x, 0.5f, pos.z));
            instance.model = glm::rotate(instance.model, angle + glm::radians(180.0f), glm::vec3(0, 1, 0));
        }

        auto& _mesh_renderer = _registry.get<MeshRenderer>(m_instance_entity);
        _mesh_renderer.set_instance_data(m_agent_instances);
    }
}
