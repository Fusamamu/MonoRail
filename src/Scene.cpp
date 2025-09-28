#include "Scene.h"

#include "Engine.h"

Scene::Scene() = default;
Scene::~Scene() = default;

void Scene::on_enter()
{
    m_registry.ctx().emplace<Camera>();

    auto& _camera = m_registry.ctx().get<Camera>();
    _camera.position = { 10.0f, 10.0f, 10.0f };

    Shader* _default_shader = ResourceManager::instance().get_shader    ("toon");
    Mesh*   _teapot         = ResourceManager::instance().get_first_mesh("teapot");

    glm::mat4 _model = glm::translate(glm::mat4(1.0f), { 0.0f, 0.0f, 0.0f });
    glm::mat4 _view  = _camera.get_view_matrix();
    glm::mat4 _proj  = _camera.get_projection_matrix();

    _default_shader->use();
    _default_shader->set_mat4_uniform_model     (_model);
    _default_shader->set_mat4_uniform_view      (_view);
    _default_shader->set_mat4_uniform_projection (_proj);

    entt::entity _e = m_registry.create();

    auto& _mesh_renderer = m_registry.emplace<MeshRenderer>(_e);
    _mesh_renderer.load_mesh      (_teapot);
    _mesh_renderer.set_buffer_data(_teapot);

    if(on_enter_callback)
        on_enter_callback();
}

void Scene::on_exit()
{
    if(on_exit_callback)
        on_exit_callback();
}

void Scene::on_update(float delta_time)
{
    m_input_system.update();

    auto& _camera = m_registry.ctx().get<Camera>();

    if (m_input_system.get_quit_requested())
        m_engine_owner->request_quit();

    if (m_input_system.is_key_held(SDL_SCANCODE_A))
        _camera.camera_move_left(0.25f);
    if (m_input_system.is_key_held(SDL_SCANCODE_D))
        _camera.camera_move_right(0.25f);
    if (m_input_system.is_key_held(SDL_SCANCODE_W))
        _camera.camera_move_up(0.25f);
    if (m_input_system.is_key_held(SDL_SCANCODE_S))
        _camera.camera_move_down(0.25f);

    glm::mat4 _view  = _camera.get_view_matrix();

    Shader* _default_shader = ResourceManager::instance().get_shader    ("toon");
    _default_shader->use();
    _default_shader->set_mat4_uniform_view      (_view);
}

void Scene::on_render(float delta_time)
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader* _default_shader = ResourceManager::instance().get_shader    ("toon");

    _default_shader->use();

    auto _mesh_view = m_registry.view<MeshRenderer>();
    for (auto _e : _mesh_view)
    {
        auto& _mesh_renderer = m_registry.get<MeshRenderer>(_e);
        _mesh_renderer.draw();
    }

    SDL_GL_SwapWindow(p_window);
    SDL_Delay(16);
}

