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

class Engine;

struct LightData
{
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
};

struct FogData
{
    alignas(16) glm::vec3 fogColor;  // 12 bytes + 4 bytes padding
    alignas(4)  float fogStart;      // 4 bytes
    alignas(4)  float fogEnd;        // 4 bytes
    alignas(4)  float fogDensity;    // 4 bytes
    alignas(4)  float pad;           // extra padding to make total size multiple of 16 bytes
};

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

    void create_object(const std::string& _name, const std::string& _mesh_name, glm::vec3 _position);

    void on_enter();
    void on_exit();
    void on_update(float delta_time);
    void on_render(float delta_time);

private:
    Engine*        m_engine_owner;
    entt::registry m_registry;
    InputSystem    m_input_system;

    unsigned int m_camera_data_ubo;
    unsigned int m_light_data_ubo;
    GLuint m_fog_data_ubo;


    FrameBuffer    m_framebuffer;
};

#endif
