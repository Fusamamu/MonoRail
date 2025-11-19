#ifndef AGENT_SYSTEM_H
#define AGENT_SYSTEM_H

#include "PCH.h"
#include "Renderer/MeshRenderer.h"
#include "Renderer/Material.h"

namespace EntitySystem
{
    class AgentSystem
    {
    public:
        AgentSystem() = default;
        ~AgentSystem() = default;

        void init        (entt::registry& _registry, glm::vec2 _bounds);
        void update      (entt::registry& _registry);
        void update_boids(entt::registry& _registry, float _dt);
    private:
        entt::entity m_instance_entity;
        std::vector<glm::vec2> m_agent_velocities;
        std::vector<InstanceData> m_agent_instances;

        glm::vec3 center = glm::vec3(0.0f);
        float max_radius = 15.0f;
    };
}

#endif 
