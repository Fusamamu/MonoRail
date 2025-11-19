#ifndef AGENTSYSTEM_H
#define AGENTSYSTEM_H

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

        void init  (entt::registry& _registry, glm::vec2 _bounds);
        void update(entt::registry& _registry);
    private:
        entt::entity m_instance_entity;
        std::vector<glm::vec2> m_agent_velocities;
        std::vector<InstanceData> m_agent_instances;
    };
}

#endif 
