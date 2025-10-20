#include "Scene.h"

#include "Engine.h"

Scene::Scene() = default;
Scene::~Scene() = default;

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

// sphere settings
glm::vec3 center = glm::vec3(0.0f);
float max_radius = 15.0f;

void update_boids(entt::registry& reg, float dt) {
    auto view = reg.view<Transform, RigidBody, Boid>();

    for (auto e : view)
    {
        auto& _tf = view.get<Transform>(e);
        auto& _rb = view.get<RigidBody>(e);

        glm::vec3 separation{0.0f};
        glm::vec3 alignment{0.0f};
        glm::vec3 cohesion{0.0f};

        int neighbor_count = 0;

        for (auto other : view)
        {
            if (other == e)
                continue;

            auto& ot = view.get<Transform>(other);
            auto& ov = view.get<RigidBody>(other);

            float dist = glm::distance(_tf.position, ot.position);
            if (dist < 10.0f) { // neighborhood radius
                separation += (_tf.position - ot.position) / (dist*dist);
                alignment  += ov.velocity;
                cohesion   += ot.position;
                neighbor_count++;
            }
        }

        if (neighbor_count > 0) {
            alignment /= neighbor_count;
            cohesion  /= neighbor_count;

            glm::vec3 cohesion_dir = cohesion - _tf.position;

            _rb.velocity += (separation * 1.5f +
                           alignment  * 1.0f +
                           cohesion_dir * 1.0f) * dt;
        }

        if (glm::length(_rb.velocity) > 0.0001f) {
            glm::vec3 forward = glm::normalize(_rb.velocity);

            float yaw   = atan2(forward.x, forward.z);
            float pitch = asin(-forward.y);
            float roll  = 0.0f;

            _tf.rotation = glm::vec3(pitch, yaw, roll);
        }

        // sphere containment
        glm::vec3 to_center = center - _tf.position;
        float dist_from_center = glm::length(to_center);
        if (dist_from_center > max_radius) {
            // steer strongly back if outside
            _rb.velocity += glm::normalize(to_center) * 5.0f * dt;
        } else if (dist_from_center > max_radius * 0.9f) {
            // gently steer back if near boundary
            _rb.velocity += glm::normalize(to_center) * 2.0f * dt;
        }

        // limit speed
        float speed = glm::length(_rb.velocity);
        if (speed > 5.0f) {
            _rb.velocity = glm::normalize(_rb.velocity) * 5.0f;
        }

        // integrate position
        _tf.position += _rb.velocity * dt;
    }
}

void Scene::on_enter()
{
    auto& _camera = m_registry.ctx().emplace<Camera>();

    _camera.position = { 20.0f, 20.0f, 20.0f };
    _camera.target   = { 0.0f, 0.0f, 0.0f };
    _camera.front    = glm::normalize(_camera.target - _camera.position);
    _camera.update_angles_from_vectors();

    auto _e_directional_light = m_registry.create();
    m_registry.emplace<DirectionalLight>(_e_directional_light);

    //create boid
    // for (int i = 0; i < 200; i++) {
    //     auto _e = m_registry.create();
    //
    //     auto& _transform  = m_registry.emplace<Transform>(_e);
    //     auto& _rigid_body = m_registry.emplace<RigidBody>(_e); // random unit direction
    //     auto& _boid       = m_registry.emplace<Boid>     (_e);
    //
    //     _transform.position = glm::vec3(
    //         randomFloat(-10.0f, 10.0f),
    //         randomFloat(-10.0f, 10.0f),
    //         randomFloat(-10.0f, 10.0f)
    //     );
    //
    //     _rigid_body.velocity = glm::sphericalRand(1.0f); // radius = 1
    //
    //     _boid.id = i;
    //
    //     Mesh* _arrow_mesh = ResourceManager::instance().get_first_mesh("arrow");
    //
    //     auto& _material_comp = m_registry.emplace<Material>(_e);
    //     _material_comp.shader_id   = "toon";
    //     _material_comp.depth_test  = true;
    //     _material_comp.depth_write = true;
    //
    //     auto& _mesh_renderer = m_registry.emplace<MeshRenderer>(_e);
    //     _mesh_renderer.load_mesh      (_arrow_mesh);
    //     _mesh_renderer.set_buffer_data(_arrow_mesh);
    // }

    m_render_pipeline.init(m_registry);

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

    create_tile_grid();

    AABB _aabb;
    _aabb.min = glm::vec3(0.0f);
    _aabb.max = glm::vec3(5.0f);
    m_gizmos_renderer.create_aabb_gizmos(_aabb);

    if(on_enter_callback)
        on_enter_callback();

    // Quad _quad;
    // screen_mesh = new Mesh();
    // *screen_mesh = _quad.screen_vertices_to_mesh();
    //
    // m_screen_mesh_renderer = new MeshRenderer();
    // m_screen_mesh_renderer->load_mesh      (screen_mesh);
    // m_screen_mesh_renderer->set_buffer_data(screen_mesh);
}

void Scene::on_exit()
{
    if(on_exit_callback)
        on_exit_callback();
}

void Scene::on_update(float delta_time)
{
    m_input_system.update();

    if (m_input_system.get_quit_requested())
        m_engine_owner->request_quit();

    auto& _camera = m_registry.ctx().get<Camera>();

    if (m_input_system.is_mouse_button_held(SDL_BUTTON_RIGHT))
    {
        glm::vec2 mouse_delta = m_input_system.get_mouse_delta(); // implement delta since last frame
        float sensitivity = 0.1f; // adjust for faster/slower rotation
        _camera.rotate(mouse_delta.x * sensitivity, -mouse_delta.y * sensitivity);

        if (m_input_system.is_key_held(SDL_SCANCODE_W)) _camera.move_forward (0.25f);
        if (m_input_system.is_key_held(SDL_SCANCODE_S)) _camera.move_backward(0.25f);
        if (m_input_system.is_key_held(SDL_SCANCODE_A)) _camera.move_left    (0.25f);
        if (m_input_system.is_key_held(SDL_SCANCODE_D)) _camera.move_right   (0.25f);
    }
    else
    {
        if (m_input_system.is_key_held(SDL_SCANCODE_A))
            _camera.camera_move_left(0.25f);
        if (m_input_system.is_key_held(SDL_SCANCODE_D))
            _camera.camera_move_right(0.25f);
        if (m_input_system.is_key_held(SDL_SCANCODE_W))
            _camera.camera_move_up(0.25f);
        if (m_input_system.is_key_held(SDL_SCANCODE_S))
            _camera.camera_move_down(0.25f);
    }

    auto& _grid = m_registry.ctx().get<Grid3D>();
    _grid.update(m_registry, _camera, m_input_system);

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

void Scene::on_render(float _dt)
{
    m_render_pipeline.render(m_registry);
}

void Scene::on_render_gui(float _dt)
{
    // glDisable(GL_DEPTH_TEST);
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //
    // MGUI::begin_window("WINDOW", { 100.0f, 100.0f }, { 500.0f, 500.0f });
    // MGUI::end_window();
    //
    // MGUI::begin_window("OTHER", { 200.0f, 100.0f }, { 500.0f, 500.0f });
    // MGUI::end_window();
    //
    // glEnable(GL_DEPTH_TEST);
    // glDisable(GL_BLEND);

    //ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Scene debug");

    ImGui::Text("FPS: %.1f", Time::fps);

    auto _directional_light_view = m_registry.view<DirectionalLight>();
    for (auto _e : _directional_light_view)
    {
        auto& _directional_light = m_registry.get<DirectionalLight>(_e);
        if (ImGui::DragFloat3("Light direction", &_directional_light.direction[0], 0.1f, -100.0f, 100.0f))
            m_render_pipeline.update_light_ubo(_directional_light);
    }

    ImGui::Checkbox("Display depth", &m_render_pipeline.display_depth);

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

    ImGui::Text("Mouse x: %.3f", static_cast<float>(MGUI::input.mouse_x));
    ImGui::Text("Mouse y: %.3f", static_cast<float>(MGUI::input.mouse_y));

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Scene::create_tile_grid()
{
    Grid3D& _grid = m_registry.ctx().emplace<Grid3D>();
    _grid.init(20, 3, 20);
    _grid.generate_tiles        (m_registry);
    _grid.create_tile_instance  (m_registry);
    _grid.generate_corner_nodes (m_registry);
    _grid.create_corner_instance(m_registry);
    _grid.store_corners_refs    (m_registry);
    _grid.store_tile_refs       (m_registry);
    _grid.fill_tile_at_level    (m_registry, 0);
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
    _material_comp.diffuseMap  = _material.diffuseMap;

    auto& _mesh_renderer = m_registry.emplace<MeshRenderer>(_e);
    _mesh_renderer.load_mesh      (_teapot);
    _mesh_renderer.set_buffer_data(_teapot);

    return _e;
}

