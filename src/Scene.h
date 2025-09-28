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

    void create_object(const std::string& name, const std::string& _mesh_name, glm::vec3 _position);

    void on_enter();
    void on_exit();
    void on_update(float delta_time);
    void on_render(float delta_time);

private:
    Engine*        m_engine_owner;
    entt::registry m_registry;
    InputSystem    m_input_system;

    unsigned int m_ubo;
};

#endif
