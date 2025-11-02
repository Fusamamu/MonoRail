#include "Scene.h"

#include "Engine.h"
#include "Navigation/AStar.h"

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
    //prototype_corners();

    m_agent_system.init(m_registry, glm::vec2(0.0f));

    // AABB _aabb;
    // _aabb.min = glm::vec3(0.0f);
    // _aabb.max = glm::vec3(5.0f);
    // m_gizmos_renderer.create_aabb_gizmos(_aabb);

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
    PROFILE_SCOPE("Update");

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
    {
        PROFILE_SCOPE("Grid update");

        switch (_grid.mode)
        {
            case Grid3D::Mode::NONE:
                break;
            case Grid3D::Mode::ADD_TILE:
                if (m_input_system.left_mouse_pressed())
                {
                    Ray _ray = _camera.screen_point_to_ray(m_input_system.get_mouse_pos(), g_app_config.screen_size());
                    float _dist = 0.0f;
                    entt::entity _entity = ray_cast_select_entity(m_registry, _ray, _dist);
                    if (_entity != entt::null)
                    {
                        if (auto [tile, transform] = m_registry.try_get<Node3D, Transform>(_entity); tile && transform)
                        {
                            if (tile->type != TileType::GROUND)
                                return;

                            NAV::Track* _track = _grid.add_track(m_registry,
                                tile     ->to_node_index(0, 1, 0),
                                transform->position + glm::vec3(0.0f, 0.5f, 0.0f));

                            if (_track)
                            {
                                m_track_graph.add_track(m_registry, _track);

                                NAV::Track* _origin_track = m_track_graph.try_get_track_at(m_registry, NodeIndex(0, 1, 0));

                                std::vector<NAV::Edge> _edges                                  = NAV::generate_edges       (m_registry, m_track_graph.tracks, m_track_graph.track_map);
                                std::unordered_map<NAV::Track*, std::vector<NAV::Track*>> _adj = NAV::build_track_adjacency(m_registry, _edges);
                                std::vector<NAV::Track*> _path                                 = NAV::a_star_search_tracks (m_registry, _origin_track, _track, _adj);

                                NAV::print_edges(m_registry, _edges);

                                std::vector<NAV::TrackNode*> _track_nodes    = NAV::translate_to_track_nodes(_path);
                                std::vector<glm::vec3>       _path_positions = NAV::translate_to_world_position(m_registry, _track_nodes);

                                m_render_pipeline.update_line_gizmos(_path_positions);
                            }
                        }
                    }
                }
                _grid.update_tile_animations(m_registry, 0.016f);
                break;
            case Grid3D::Mode::REMOVE_TILE:
                break;
            case Grid3D::Mode::MARK_TILE:
                break;
            case Grid3D::Mode::ADD_AGENT:
                break;
        }

        _grid.update(m_registry, _camera, m_input_system);
    }

    auto _create_agent = [&]()
    {
        std::optional<std::vector<entt::entity>> _path = _grid.find_path(m_registry, NodeIndex(0, 1, 0), NodeIndex(10, 1, 10));
        if (_path.has_value())
        {
            Material _material;
            _material.shader_id = "toon";
            entt::entity _e = create_object("agent", "bevel_cube", glm::vec3(0.0f, 0.0f, 0.0f), _material);

            Agent& _agent = m_registry.emplace<Agent>(_e);

            _agent.target_path.reserve(_path.value().size());

            for (entt::entity _entity : _path.value())
            {
                if (Node3D* _node = m_registry.try_get<Node3D>(_entity))
                {
                    float _x = static_cast<float>(_node->idx);
                    float _y = static_cast<float>(_node->idy);
                    float _z = static_cast<float>(_node->idz);

                    glm::vec3 _path_position = glm::vec3(_x, _y, _z);

                    std::cout << _path_position.x << ", " << _path_position.y << ", " << _path_position.z << std::endl;

                    _agent.target_path.emplace_back(_path_position);
                    _agent.following_path = true;
                }
            }
        }
    };

    if (m_input_system.is_key_down(SDL_SCANCODE_SPACE))
    {
        _create_agent();
    }

    auto _agent_view = m_registry.view<Transform, Agent>();
    for (auto _e : _agent_view)
    {
        auto& _transform = m_registry.get<Transform>(_e);
        auto& _agent     = m_registry.get<Agent>    (_e);
        _agent.update(_transform, Time::delta_f/1000.0f);
    }

    m_agent_system.update(m_registry);

    update_scene_graph();
}

void Scene::on_render(float _dt)
{
    PROFILE_SCOPE("Render");
    m_render_pipeline.render(m_registry);
}

void Scene::on_render_gui(float _dt)
{
    PROFILE_SCOPE("RenderGUI");
    //ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Scene debug");

    ImGui::Text("FPS: %.1f"       , Time::fps);
    ImGui::Text("Time delta: %.4f", Time::delta_f);

    auto _directional_light_view = m_registry.view<DirectionalLight>();
    for (auto _e : _directional_light_view)
    {
        auto& _directional_light = m_registry.get<DirectionalLight>(_e);

        if (ImGui::Checkbox("Cast shadows", &_directional_light.cast_shadow))
        {

        }

        if (ImGui::DragFloat3("Light direction", &_directional_light.direction[0], 0.1f, -100.0f, 100.0f))
            m_render_pipeline.update_light_ubo(_directional_light);
        if (ImGui::DragFloat3("Light position", &_directional_light.position[0],  0.1f, -100.0f, 100.0f))
            m_render_pipeline.update_light_ubo(_directional_light);
        if (ImGui::DragFloat("Light orthograph size", &_directional_light.orthographic_size, 0.01f, 25.0f, 100.0f))
            m_render_pipeline.update_light_ubo(_directional_light);
    }

    ImGui::Checkbox("Display depth"     , &m_render_pipeline.display_depth);
    ImGui::Checkbox("Display shadow map", &m_render_pipeline.display_shadow_map);
    ImGui::Checkbox("Display DOF"       , &m_render_pipeline.display_dof);

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

    if (ImGui::DragFloat("focus distance", &_camera.focus_distance))
    {
        Shader* _dof = ResourceManager::instance().get_shader("depth_of_field");
        _dof->use();
        _dof->set_float("u_focus_dist", _camera.focus_distance);
    }

    if (ImGui::DragFloat("focus range", &_camera.focus_range))
    {
        Shader* _dof = ResourceManager::instance().get_shader("depth_of_field");
        _dof->use();
        _dof->set_float("u_focus_range", _camera.focus_range);
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

    auto& _grid = m_registry.ctx().get<Grid3D>();
    const char* DirectionNames[] = { "NONE", "ADD TILE", "REMOVE TILE", "MARK TILE", "ADD AGENT" };
    int current = static_cast<int>(_grid.mode);
    if (ImGui::Combo("Tile mode", &current, DirectionNames, IM_ARRAYSIZE(DirectionNames))) {
        _grid.mode = static_cast<Grid3D::Mode>(current);
    }

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

    auto& _node          = m_registry.emplace<Node>          (_e, Node());
    auto& _transform     = m_registry.emplace<Transform>     (_e);
    auto& _aabb          = m_registry.emplace<AABB>          (_e, AABB());
    auto& _material_comp = m_registry.emplace<Material>      (_e);
    auto& _mesh_renderer = m_registry.emplace<MeshRenderer>  (_e);
    auto& _gizmos        = m_registry.emplace<GizmosRenderer>(_e);

    _node.name = _name;

    _transform.position = _position;

    _aabb.min = { -0.5f, -0.5f, -0.5f };
    _aabb.max = {  0.5f,  0.5f,  0.5f };

    _material_comp.shader_id   = _material.shader_id;
    _material_comp.depth_test  = _material.depth_test;
    _material_comp.depth_write = _material.depth_write;
    _material_comp.diffuseMap  = _material.diffuseMap;

    _mesh_renderer.load_mesh      (_teapot);
    _mesh_renderer.set_buffer_data(_teapot);

    _gizmos.create_aabb_gizmos(_aabb);

    return _e;
}

void Scene::prototype_corners()
{
    Material _phong_material;
    _phong_material.shader_id    = "phong";
    _phong_material.diffuse_color = glm::vec3(1.0f, 1.0f, 1.0f);

    entt::entity _e = create_object("corner", "c_1000_0000", glm::vec3(0.0f), _phong_material);
}

