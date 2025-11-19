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
        const float _move_speed   = 5.0f;  // units per second
        const float _turn_chance  = 0.005f; // 1% chance to change direction per frame

        const glm::vec2 _bounds (20.0f, 20.0f);

        float dt = 0.0016f;

        for (size_t i = 0; i < m_agent_instances.size(); ++i)
        {
            InstanceData& instance = m_agent_instances[i];

            glm::vec2& dir = m_agent_velocities[i];

            // Occasionally change direction
            if ((float)std::rand() / RAND_MAX < _turn_chance)
            {
                glm::vec2 newDir(
                    ((float)std::rand() / RAND_MAX - 0.5f) * 2.0f,
                    ((float)std::rand() / RAND_MAX - 0.5f) * 2.0f
                );
                dir = glm::normalize(newDir);
            }

            // Extract current position from model matrix
            glm::vec3 pos = instance.model[3]; // column 3 = translation
            pos.x += dir.x * _move_speed * dt;
            pos.z += dir.y * _move_speed * dt;

            // Bounce off edges
            float halfX = _bounds.x * 0.5f;
            float halfZ = _bounds.y * 0.5f;

            if (pos.x < 0.0f || pos.x > _bounds.x)
            {
                dir.x = -dir.x;
                pos.x = glm::clamp(pos.x, 0.0f, _bounds.x);
            }

            if (pos.z < 0.0f || pos.z > _bounds.y)
            {
                dir.y = -dir.y;
                pos.z = glm::clamp(pos.z, 0.0f, _bounds.y);
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

    void AgentSystem::update_boids(entt::registry& _registry, float _dt)
    {
        auto _view = _registry.view<Component::Transform, RigidBody, Boid>();

        for (auto e : _view)
        {
            auto& _tf = _view.get<Component::Transform>(e);
            auto& _rb  = _view.get<RigidBody>           (e);

            glm::vec3 separation{0.0f};
            glm::vec3 alignment {0.0f};
            glm::vec3 cohesion  {0.0f};

            int neighbor_count = 0;

            for (auto _other : _view)
            {
                if (_other == e)
                    continue;

                auto& ot = _view.get<Component::Transform>(_other);
                auto& ov  = _view.get<RigidBody>           (_other);

                float dist = glm::distance(_tf.position, ot.position);
                if (dist < 10.0f)
                {
                    separation += (_tf.position - ot.position) / (dist*dist);
                    alignment  += ov.velocity;
                    cohesion   += ot.position;
                    neighbor_count++;
                }
            }

            if (neighbor_count > 0)
            {
                alignment /= neighbor_count;
                cohesion  /= neighbor_count;

                glm::vec3 cohesion_dir = cohesion - _tf.position;

                _rb.velocity += (separation * 1.5f +
                               alignment  * 1.0f +
                               cohesion_dir * 1.0f) * _dt;
            }

            if (glm::length(_rb.velocity) > 0.0001f)
            {
                glm::vec3 forward = glm::normalize(_rb.velocity);

                float yaw   = atan2(forward.x, forward.z);
                float pitch = asin(-forward.y);
                float roll  = 0.0f;

                _tf.rotation = glm::vec3(pitch, yaw, roll);
            }

            glm::vec3 to_center = center - _tf.position;
            float dist_from_center = glm::length(to_center);
            if (dist_from_center > max_radius) {
                _rb.velocity += glm::normalize(to_center) * 5.0f * _dt;
            } else if (dist_from_center > max_radius * 0.9f) {
                _rb.velocity += glm::normalize(to_center) * 2.0f * _dt;
            }

            float speed = glm::length(_rb.velocity);
            if (speed > 5.0f) {
                _rb.velocity = glm::normalize(_rb.velocity) * 5.0f;
            }

            _tf.position += _rb.velocity * _dt;
        }
    }
}
