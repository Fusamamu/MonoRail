#ifndef SCENE_H
#define SCENE_H

#include "PCH.h"
#include "Camera.h"
#include "InputSystem.h"
#include "Shader.h"
#include "Mesh.h"
#include "ResourceManager.h"
#include "MeshRenderer.h"
#include "Grid.h"
#include "FrameBuffer.h"
#include "GizmosRenderer.h"
#include "RenderPipeline.h"
#include "ApplicationConfig.h"

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

    entt::entity create_object(const std::string& _name, const std::string& _mesh_name, glm::vec3 _position, const Material& _material);

    void on_enter();
    void on_exit();
    void on_update(float delta_time);
    void on_render(float delta_time);

private:
    entt::registry m_registry;
    Engine*        m_engine_owner;

    RenderPipeline m_render_pipeline;
    InputSystem    m_input_system;

    GLuint m_fog_data_ubo;
    FogData m_fog_data;
    GizmosRenderer m_gizmos_renderer;

    void update_world_transform(entt::entity _entity, const glm::mat4& _parent_world)
    {
        auto &_transform = m_registry.get<Transform>(_entity);

        _transform.world_mat = _parent_world * _transform.get_local_mat4();

        auto _view = m_registry.view<Parent>();

        for (auto _child : _view)
        {
            if (_view.get<Parent>(_child).entity == _entity)
            {
                update_world_transform(_child, _transform.world_mat);
            }
        }
    }
};

#endif
