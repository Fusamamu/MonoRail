#ifndef SCENE_H
#define SCENE_H

#include "PCH.h"
#include "Camera.h"
#include "InputSystem.h"
#include "Shader.h"
#include "Core/Mesh.h"
#include "Asset/AssetManager.h"
#include "MeshRenderer.h"
#include "Grid3D.h"
#include "FrameBuffer.h"
#include "GizmosRenderer.h"
#include "RenderPipeline.h"
#include "ApplicationConfig.h"
#include "Ray.h"
#include "Time.h"
#include "AgentSystem.h"
#include "Navigation/Navigation.h"
#include "Navigation/Agent.h"

class Engine;

class Scene {
public:
    SDL_Window*   p_window;

    std::string name;
    Scene(const std::string& name, Engine* _engine) :
        name(name),
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
    Engine*        m_engine_owner;

    RenderPipeline m_render_pipeline;
    InputSystem    m_input_system;
    AgentSystem    m_agent_system;

    GLuint  m_fog_data_ubo;
    FogData m_fog_data;
    GizmosRenderer m_gizmos_renderer;

    entt::entity m_train_entity;
    NAV::TrackGraph m_track_graph;
    std::vector<glm::vec3> m_track_paths;

    void update_scene_graph()
    {
        PROFILE_SCOPE("Scene graph");
        auto _roots = m_registry.view<Node, Transform>(entt::exclude<Parent>);

        for (auto _e : _roots)
        {
            auto& _node = m_registry.get<Node>(_e);
            if (_node.is_static)
                continue;
            update_world_transform(_e, glm::mat4(1.0f));
        }
    }

    void update_world_transform(entt::entity _entity, const glm::mat4& _parent_world)
    {
        PROFILE_SCOPE("Scene transform");
        auto& _node      = m_registry.get<Node>     (_entity);
        auto& _transform = m_registry.get<Transform>(_entity);

        if (!_node.is_static || _node.is_dirty)
            _transform.world_mat = _parent_world * _transform.get_local_mat4();

        auto _view = m_registry.view<Parent>();
        for (auto _child : _view)
        {
            if (_view.get<Parent>(_child).entity == _entity)
                update_world_transform(_child, _transform.world_mat);
        }
    }
};

#endif
