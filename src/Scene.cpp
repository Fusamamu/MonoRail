#include "Scene.h"

#include "Engine.h"

Scene::Scene() = default;
Scene::~Scene() = default;

void Scene::on_enter()
{
    m_registry.ctx().emplace<Camera>();
    m_registry.ctx().emplace<Grid>  ();

    auto& _camera = m_registry.ctx().get<Camera>();
    _camera.position = { 10.0f, 10.0f, 10.0f };

    glm::mat4 _view  = _camera.get_view_matrix();
    glm::mat4 _proj  = _camera.get_projection_matrix();

    Shader* _default_shader = ResourceManager::instance().get_shader("toon");
    _default_shader->block_bind("Matrices", 0);

    glGenBuffers(1, &m_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, m_ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_ubo, 0, 2 * sizeof(glm::mat4));

    glBindBuffer   (GL_UNIFORM_BUFFER, m_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0                , sizeof(glm::mat4), glm::value_ptr(_proj));
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_view));
    glBindBuffer   (GL_UNIFORM_BUFFER, 0);

    if(on_enter_callback)
        on_enter_callback();
}

void Scene::on_exit()
{
    if(on_exit_callback)
        on_exit_callback();

    if(m_ubo)
        glDeleteBuffers(1, &m_ubo);
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

    glBindBuffer   (GL_UNIFORM_BUFFER, m_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_view));
    glBindBuffer   (GL_UNIFORM_BUFFER, 0);
}

void Scene::on_render(float delta_time)
{
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader* _default_shader = ResourceManager::instance().get_shader    ("toon");
    _default_shader->use();

    auto _mesh_view = m_registry.view<Transform, MeshRenderer>();
    for (auto _e : _mesh_view)
    {
        auto& _transform     = m_registry.get<Transform>   (_e);
        auto& _mesh_renderer = m_registry.get<MeshRenderer>(_e);

        _default_shader->set_mat4_uniform_model(glm::translate(glm::mat4(1.0f), _transform.position));

        _mesh_renderer.draw();
    }

    SDL_GL_SwapWindow(p_window);
    SDL_Delay(16);
}

void Scene::create_object(const std::string& name, const std::string& _mesh_name, glm::vec3 _position)
{
    entt::entity _e = m_registry.create();

    Mesh* _teapot = ResourceManager::instance().get_first_mesh(_mesh_name);

    auto& _transform = m_registry.emplace<Transform>(_e);
    _transform.position = _position;

    auto& _mesh_renderer = m_registry.emplace<MeshRenderer>(_e);
    _mesh_renderer.load_mesh      (_teapot);
    _mesh_renderer.set_buffer_data(_teapot);
}

