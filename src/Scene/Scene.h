#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <functional>
#include <SDL2/SDL.h>

#include "InputSystem.h"

#include "Renderer/Shader.h"
#include "Renderer/MeshRenderer.h"
#include "Renderer/GizmosRenderer.h"
#include "Renderer/RenderPipeline.h"

#include "EntitySystem/AgentSystem.h"

#include "Navigation/Navigation.h"

namespace MUG
{
class Engine;

}

class Scene {
public:
    SDL_Window*   p_window;

    std::string name;

    Scene(const std::string& _name, MUG::Engine* _engine) :
        name          (_name  ),
        m_engine_owner(_engine)
    {

    }
    Scene();
    ~Scene();

    std::function<void()> on_enter_callback;
    std::function<void()> on_exit_callback;

    entt::registry& get_registry(){
        return m_registry;
    }

    void on_enter();
    void on_exit();
    void on_update(float delta_time);
    void on_render(float _dt);
    void on_render_gui(float _dt);

    entt::entity create_object(const std::string& _name, const std::string& _mesh_name, glm::vec3 _position, const Material& _material);

    void create_tile_grid();
    void prototype_corners();
private:
    entt::registry m_registry;
    MUG::Engine*        m_engine_owner;

    RenderPipeline m_render_pipeline;
    InputSystem    m_input_system;

    EntitySystem::AgentSystem m_agent_system;

    GLuint         m_fog_data_ubo;
    FogData        m_fog_data;
    GizmosRenderer m_gizmos_renderer;

    entt::entity m_train_entity;
    NAV::TrackGraph m_track_graph;
    std::vector<glm::vec3> m_track_paths;

    void update_scene_graph();
    void update_world_transform(entt::entity _entity, const glm::mat4& _parent_world);
};

#endif
