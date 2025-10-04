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

    Shader* _default_shader = ResourceManager::instance().get_shader("phong");
    _default_shader->use();
    _default_shader->block_bind("CameraData"           , 0);
    _default_shader->block_bind("DirectionalLightBlock", 1);
    _default_shader->block_bind("FogDataBlock"         , 2);
    _default_shader->set_float("u_shininess", 100.0f);

    Shader* _fog_plane_shader = ResourceManager::instance().get_shader("fog_plane");
    _fog_plane_shader->use();
    _fog_plane_shader->block_bind("CameraData"           , 0);
    _fog_plane_shader->block_bind("DirectionalLightBlock", 1);
    _fog_plane_shader->block_bind("FogDataBlock"         , 2);
    _fog_plane_shader->set_float("u_shininess", 100.0f);
    _fog_plane_shader->set_vec2("u_screen_size", glm::vec2(1600, 1200));


    glGenBuffers(1, &m_camera_data_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, m_camera_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    CameraData camera_data = _camera.get_camera_data();

    glBindBuffer   (GL_UNIFORM_BUFFER, m_camera_data_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), &camera_data);
    glBindBuffer   (GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_camera_data_ubo);

    //Light
    glGenBuffers(1, &m_light_data_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, m_light_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_light_data.direction = glm::normalize(glm::vec3( 0.1f, -0.6f, -0.8f));
    m_light_data.ambient   = glm::vec3(0.2f, 0.2f, 0.2f);
    m_light_data.diffuse   = glm::vec3(0.5f, 0.5f, 0.5f);
    m_light_data.specular  = glm::vec3(1.0f, 1.0f, 1.0f);

    glBindBuffer   (GL_UNIFORM_BUFFER, m_light_data_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData), &m_light_data);
    glBindBuffer   (GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_light_data_ubo);

    //Fog
    glGenBuffers(1, &m_fog_data_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, m_fog_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(FogData), nullptr, GL_STATIC_DRAW); // or nullptr if updating later
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    m_fog_data.fogColor   = glm::vec3(0.5f, 0.5f, 0.5f); // gray fog
    m_fog_data.fogStart   = -10.0f;
    m_fog_data.fogEnd     = -4.0f;
    m_fog_data.fogDensity = 3.0f;
    m_fog_data.pad        = 0.0f; // padding must be set

    glBindBuffer   (GL_UNIFORM_BUFFER, m_fog_data_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FogData), &m_fog_data);
    glBindBuffer   (GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_fog_data_ubo);

    auto& _grid = m_registry.ctx().get<Grid>();
    _grid.generate_tiles_with_perlin(m_registry);

    m_framebuffer = FrameBuffer(g_app_config.SCREEN_WIDTH, g_app_config.SCREEN_HEIGHT, true);
    m_framebuffer.init();
    m_framebuffer.attach_color_texture();
    m_framebuffer.attach_depth_texture();

    m_depth_framebuffer = FrameBuffer(1600, 1200, true);
    m_depth_framebuffer.init();
    m_depth_framebuffer.attach_color_texture();
    m_depth_framebuffer.attach_depth_texture();

    Shader* _screen_quad = ResourceManager::instance().get_shader("screen_quad");
    _screen_quad->use();
    _screen_quad->set_int("u_screen_texture", 0);

    Shader* _depth_quad = ResourceManager::instance().get_shader("depth_quad");
    _depth_quad->use();
    _depth_quad->set_int("u_depth_texture", 0);
    _depth_quad->set_float("near_plane", 1);
    _depth_quad->set_float("far_plane", 550);

    AABB _aabb;
    _aabb.min = glm::vec3(0.0f);
    _aabb.max = glm::vec3(5.0f);

    m_gizmos_renderer.create_aabb_gizmos(_aabb);

    if(on_enter_callback)
        on_enter_callback();


    m_render_pipeline.init();
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

    auto _agent_view = m_registry.view<Transform, Agent>();
    for (auto _e : _agent_view)
    {
        auto& _transform = m_registry.get<Transform>(_e);
        auto& _agent     = m_registry.get<Agent>    (_e);

        _agent.update_move(0.016f, _transform);

        _transform.position.x += _agent.move_direction.x * _agent.move_amount;
    }

    auto _roots = m_registry.view<Transform>(entt::exclude<Parent>);
    for (auto _e : _roots)
        update_world_transform(_e, glm::mat4(1.0f));
}

void Scene::on_render(float delta_time)
{

    m_render_pipeline.render(m_registry);

      //ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Scene debug");
    if (ImGui::DragFloat3("Light direction", &m_light_data.direction[0], 0.1f, -100.0f, 100.0f))
    {
        m_light_data.direction = glm::normalize(m_light_data.direction);
        glBindBuffer   (GL_UNIFORM_BUFFER, m_light_data_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData), &m_light_data);
        glBindBuffer   (GL_UNIFORM_BUFFER, 0);
    }

    ImGui::Checkbox("Display depth", &m_display_depth);

    auto& _camera = m_registry.ctx().get<Camera>();

    if (ImGui::DragFloat("near", &_camera.near_plane))
    {
        Shader* _screen_quad = ResourceManager::instance().get_shader("depth_quad");
        _screen_quad->use();
        _screen_quad->set_float("near_plane", _camera.near_plane);


        Shader* _fog_plane = ResourceManager::instance().get_shader("fog_plane");
        _fog_plane->use();
        _fog_plane->set_float("u_near_plane", _camera.near_plane);
    }

    if (ImGui::DragFloat("far", &_camera.far_plane))
    {
        Shader* _screen_quad = ResourceManager::instance().get_shader("depth_quad");
        _screen_quad->use();
        _screen_quad->set_float("far_plane", _camera.far_plane);


        Shader* _fog_plane = ResourceManager::instance().get_shader("fog_plane");
        _fog_plane->use();
        _fog_plane->set_float("u_far_plane", _camera.far_plane);
    }

    if (ImGui::ColorEdit3("Fog color", (float*)&m_fog_data.fogColor[0]))
    {
        glBindBuffer   (GL_UNIFORM_BUFFER, m_fog_data_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FogData), &m_fog_data);
        glBindBuffer   (GL_UNIFORM_BUFFER, 0);
    }

    if (ImGui::DragFloat("Frog start", &m_fog_data.fogStart))
    {
        glBindBuffer   (GL_UNIFORM_BUFFER, m_fog_data_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FogData), &m_fog_data);
        glBindBuffer   (GL_UNIFORM_BUFFER, 0);
    }

    if (ImGui::DragFloat("Frog end", &m_fog_data.fogEnd))
    {
        glBindBuffer   (GL_UNIFORM_BUFFER, m_fog_data_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FogData), &m_fog_data);
        glBindBuffer   (GL_UNIFORM_BUFFER, 0);
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(p_window);
    SDL_Delay(16);


    return;

    // m_depth_framebuffer.bind();
    //
    // glViewport  (0, 0, 1600, 1200);
    // glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    // glClear     (GL_DEPTH_BUFFER_BIT);
    //
    // auto _mesh_view_0 = m_registry.view<Transform, MeshRenderer, Material>();
    // for (auto _e : _mesh_view_0)
    // {
    //     auto& _transform     = m_registry.get<Transform>   (_e);
    //     auto& _mesh_renderer = m_registry.get<MeshRenderer>(_e);
    //     auto& _material      = m_registry.get<Material>    (_e);
    //
    //     Shader* _default_shader = ResourceManager::instance().get_shader(_material.shader_id);
    //     _default_shader->use();
    //     _default_shader->set_mat4_uniform_model(_transform.world_mat);
    //
    //     if (_material.depth_write)
    //         _mesh_renderer.draw();
    // }
    //
    // m_depth_framebuffer.unbind();
    //
    //
    //
    //
    //
    //
    // m_framebuffer.bind();
    //
    // glViewport(0, 0, 1600, 1200);
    // glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    // auto _mesh_view = m_registry.view<Transform, MeshRenderer, Material>();
    // for (auto _e : _mesh_view)
    // {
    //     auto& _transform     = m_registry.get<Transform>   (_e);
    //     auto& _mesh_renderer = m_registry.get<MeshRenderer>(_e);
    //     auto& _material      = m_registry.get<Material>    (_e);
    //
    //     Shader* _default_shader = ResourceManager::instance().get_shader(_material.shader_id);
    //     _default_shader->use();
    //     //_default_shader->set_mat4_uniform_model(glm::translate(glm::mat4(1.0f), _transform.position));
    //     _default_shader->set_mat4_uniform_model(_transform.world_mat);
    //
    //     if (!_material.depth_write)
    //     {
    //         glDepthMask(GL_FALSE);
    //
    //         auto& _camera = m_registry.ctx().get<Camera>();
    //
    //
    //         _default_shader->use();
    //
    //
    //
    //
    //         _default_shader->set_int("u_color_texture", 0);
    //         _default_shader->set_int("u_depth_texture", 1);
    //         _default_shader->set_float("u_near_plane", _camera.near_plane);
    //         _default_shader->set_float("u_far_plane" , _camera.far_plane);
    //
    //         glActiveTexture(GL_TEXTURE0);
    //         glBindTexture(GL_TEXTURE_2D, m_depth_framebuffer.get_color_texture());
    //
    //         glActiveTexture(GL_TEXTURE1);
    //         glBindTexture(GL_TEXTURE_2D, m_depth_framebuffer.get_depth_texture());
    //     }
    //
    //
    //
    //     _mesh_renderer.draw();
    //
    //     if (!_material.depth_write)
    //         glDepthMask(GL_TRUE);    // restore
    // }
    //
    // Shader* _aabb_gizmos = ResourceManager::instance().get_shader("aabb");
    // _aabb_gizmos->use();
    // m_gizmos_renderer.draw();
    //
    //
    // m_framebuffer.unbind();
    //
    //
    //
    //
    //
    //
    //
    //
    // glViewport(0, 0, 1600 * 2, 1200 * 2);
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //
    //
    // Quad _quad;
    // Mesh _mesh = _quad.screen_vertices_to_mesh();
    //
    // MeshRenderer _mesh_renderer;
    // _mesh_renderer.load_mesh      (&_mesh);
    // _mesh_renderer.set_buffer_data(&_mesh);
    //
    // if (!m_display_depth)
    // {
    //     Shader* _screen_quad = ResourceManager::instance().get_shader("screen_quad");
    //     _screen_quad->use();
    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, m_framebuffer.get_color_texture());
    // }
    // else
    // {
    //     Shader* _depth_quad = ResourceManager::instance().get_shader("depth_quad");
    //     _depth_quad->use();
    //     glActiveTexture(GL_TEXTURE0);
    //     glBindTexture(GL_TEXTURE_2D, m_depth_framebuffer.get_depth_texture());
    // }
    //
    // _mesh_renderer.draw();
    //
    //
    //
    //
    //
    //
    //
    //
    //
    // //ImGui
    // ImGui_ImplOpenGL3_NewFrame();
    // ImGui_ImplSDL2_NewFrame();
    // ImGui::NewFrame();
    //
    // ImGui::Begin("Scene debug");
    // if (ImGui::DragFloat3("Light direction", &m_light_data.direction[0], 0.1f, -100.0f, 100.0f))
    // {
    //     m_light_data.direction = glm::normalize(m_light_data.direction);
    //     glBindBuffer   (GL_UNIFORM_BUFFER, m_light_data_ubo);
    //     glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData), &m_light_data);
    //     glBindBuffer   (GL_UNIFORM_BUFFER, 0);
    // }
    //
    // ImGui::Checkbox("Display depth", &m_display_depth);
    //
    // auto& _camera = m_registry.ctx().get<Camera>();
    //
    // if (ImGui::DragFloat("near", &_camera.near_plane))
    // {
    //     Shader* _screen_quad = ResourceManager::instance().get_shader("depth_quad");
    //     _screen_quad->use();
    //     _screen_quad->set_float("near_plane", _camera.near_plane);
    //
    //
    //     Shader* _fog_plane = ResourceManager::instance().get_shader("fog_plane");
    //     _fog_plane->use();
    //     _fog_plane->set_float("u_near_plane", _camera.near_plane);
    // }
    //
    // if (ImGui::DragFloat("far", &_camera.far_plane))
    // {
    //     Shader* _screen_quad = ResourceManager::instance().get_shader("depth_quad");
    //     _screen_quad->use();
    //     _screen_quad->set_float("far_plane", _camera.far_plane);
    //
    //
    //     Shader* _fog_plane = ResourceManager::instance().get_shader("fog_plane");
    //     _fog_plane->use();
    //     _fog_plane->set_float("u_far_plane", _camera.far_plane);
    // }
    //
    // if (ImGui::ColorEdit3("Fog color", (float*)&m_fog_data.fogColor[0]))
    // {
    //     glBindBuffer   (GL_UNIFORM_BUFFER, m_fog_data_ubo);
    //     glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FogData), &m_fog_data);
    //     glBindBuffer   (GL_UNIFORM_BUFFER, 0);
    // }
    //
    // if (ImGui::DragFloat("Frog start", &m_fog_data.fogStart))
    // {
    //     glBindBuffer   (GL_UNIFORM_BUFFER, m_fog_data_ubo);
    //     glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FogData), &m_fog_data);
    //     glBindBuffer   (GL_UNIFORM_BUFFER, 0);
    // }
    //
    // if (ImGui::DragFloat("Frog end", &m_fog_data.fogEnd))
    // {
    //     glBindBuffer   (GL_UNIFORM_BUFFER, m_fog_data_ubo);
    //     glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FogData), &m_fog_data);
    //     glBindBuffer   (GL_UNIFORM_BUFFER, 0);
    // }
    //
    // ImGui::End();
    //
    // ImGui::Render();
    // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    //
    // SDL_GL_SwapWindow(p_window);
    // SDL_Delay(16);
}

entt::entity Scene::create_object(const std::string& _name, const std::string& _mesh_name, glm::vec3 _position, const Material& _material)
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

    auto& _material_comp = m_registry.emplace<Material>(_e);
    _material_comp.shader_id   = _material.shader_id;
    _material_comp.depth_test  = _material.depth_test;
    _material_comp.depth_write = _material.depth_write;

    auto& _mesh_renderer = m_registry.emplace<MeshRenderer>(_e);
    _mesh_renderer.load_mesh      (_teapot);
    _mesh_renderer.set_buffer_data(_teapot);

    return _e;
}

