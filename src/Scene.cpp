#include "Scene.h"

#include "Engine.h"

Scene::Scene() = default;
Scene::~Scene() = default;

void Scene::on_enter()
{
    m_registry.ctx().emplace<Camera>();
    m_registry.ctx().emplace<Grid>  (Grid(12, 12));

    auto& _camera = m_registry.ctx().get<Camera>();
    _camera.position = { 20.0f, 20.0f, 20.0f };

    glm::mat4 _view  = _camera.get_view_matrix();
    glm::mat4 _proj  = _camera.get_projection_matrix();

    Shader* _default_shader = ResourceManager::instance().get_shader("toon");
    // _default_shader->block_bind("CameraBlock", 0);

    // unsigned int uniform_block_index  = glGetUniformBlockIndex(_default_shader->id, "CameraData");
    // glUniformBlockBinding(_default_shader->id, uniform_block_index, 0);

    //this work !!!
    // glGenBuffers(1, &m_camera_data_ubo);
    // glBindBuffer(GL_UNIFORM_BUFFER, m_camera_data_ubo);
    // glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    // glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_camera_data_ubo, 0, 2 * sizeof(glm::mat4));
    //
    // glBindBuffer   (GL_UNIFORM_BUFFER, m_camera_data_ubo);
    // glBufferSubData(GL_UNIFORM_BUFFER, 0                , sizeof(glm::mat4), glm::value_ptr(_proj));
    // glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_view));
    // glBindBuffer   (GL_UNIFORM_BUFFER, 0);


    //this doesn't work!!!
    glGenBuffers(1, &m_camera_data_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, m_camera_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    CameraData camera_data = _camera.get_camera_data();

    glBindBuffer(GL_UNIFORM_BUFFER, m_camera_data_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), &camera_data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);


    // // Generate buffer
    // glGenBuffers(1, &m_camera_data_ubo);
    // glBindBuffer(GL_UNIFORM_BUFFER, m_camera_data_ubo);
    // glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), nullptr, GL_DYNAMIC_DRAW);
    // glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //
    // // Fill data
    // CameraData camera_data{};
    // camera_data.projection = _camera.get_projection_matrix();
    // camera_data.view       = _camera.get_view_matrix();
    //
    // glBindBuffer(GL_UNIFORM_BUFFER, m_camera_data_ubo);
    // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), &camera_data);
    // glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_camera_data_ubo);




    // glGenBuffers(1, &m_light_data_ubo);
    // glBindBuffer(GL_UNIFORM_BUFFER, m_light_data_ubo);
    // glBufferData(GL_UNIFORM_BUFFER, sizeof(Light), nullptr, GL_STATIC_DRAW);
    // glBindBuffer(GL_UNIFORM_BUFFER, 0);



    // GLuint fogUBO;
    // glGenBuffers(1, &fogUBO);
    // glBindBuffer(GL_UNIFORM_BUFFER, fogUBO);
    // glBufferData(GL_UNIFORM_BUFFER, sizeof(FogData), nullptr, GL_DYNAMIC_DRAW);
    // glBindBuffer(GL_UNIFORM_BUFFER, 0);
    //
    // glBindBufferBase(GL_UNIFORM_BUFFER, 0, fogUBO);// Bind UBO to binding point 0


    auto& _grid = m_registry.ctx().get<Grid>();
    _grid.generate_tiles_with_perlin(m_registry);

    m_framebuffer = FrameBuffer(2 * 1600, 2 * 1200, true);
    m_framebuffer.init();

    Shader* _screen_quad = ResourceManager::instance().get_shader("screen_quad");
    _screen_quad->use();
    _screen_quad->set_int("u_screen_texture", 0);

    if(on_enter_callback)
        on_enter_callback();
}

void Scene::on_exit()
{
    if(on_exit_callback)
        on_exit_callback();

    if(m_camera_data_ubo)
        glDeleteBuffers(1, &m_camera_data_ubo);
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

    glBindBuffer   (GL_UNIFORM_BUFFER, m_camera_data_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_view));
    glBindBuffer   (GL_UNIFORM_BUFFER, 0);

    auto _object_view = m_registry.view<Transform, AABB>();
    for (auto _e : _object_view)
    {
        auto& _transform = m_registry.get<Transform>(_e);
        auto& _aabb      = m_registry.get<AABB>     (_e);
    }
}

void Scene::on_render(float delta_time)
{
    m_framebuffer.bind();

    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader* _default_shader = ResourceManager::instance().get_shader("toon");
    _default_shader->use();

    auto _mesh_view = m_registry.view<Transform, MeshRenderer>();
    for (auto _e : _mesh_view)
    {
        auto& _transform     = m_registry.get<Transform>   (_e);
        auto& _mesh_renderer = m_registry.get<MeshRenderer>(_e);

        _default_shader->set_mat4_uniform_model(glm::translate(glm::mat4(1.0f), _transform.position));

        _mesh_renderer.draw();
    }

    m_framebuffer.unbind();

    glViewport(0, 0, 1600 * 2, 1200 * 2);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Quad _quad;
    Mesh _mesh = _quad.screen_vertices_to_mesh();
    MeshRenderer _mesh_renderer;
    _mesh_renderer.load_mesh(&_mesh);
    _mesh_renderer.set_buffer_data(&_mesh);

    Shader* _screen_quad = ResourceManager::instance().get_shader("screen_quad");
    _screen_quad->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_framebuffer.get_color_texture());

    _mesh_renderer.draw();

    SDL_GL_SwapWindow(p_window);
    SDL_Delay(16);
}

void Scene::create_object(const std::string& _name, const std::string& _mesh_name, glm::vec3 _position)
{
    entt::entity _e = m_registry.create();

    Mesh* _teapot = ResourceManager::instance().get_first_mesh(_mesh_name);

    auto& _node = m_registry.emplace<Node>(_e, Node());
    _node.name = _name;

    auto& _transform = m_registry.emplace<Transform>(_e);
    _transform.position = _position;

    auto& _aabb = m_registry.emplace<AABB>(_e, AABB());
    _aabb.min = { -0.5f, -0.5f, -0.5f };
    _aabb.max = {  0.5f,  0.5f,  0.5f };

    auto& _material = m_registry.emplace<Material>(_e);
    _material.shader_id = "toon";

    auto& _mesh_renderer = m_registry.emplace<MeshRenderer>(_e);
    _mesh_renderer.load_mesh      (_teapot);
    _mesh_renderer.set_buffer_data(_teapot);
}

