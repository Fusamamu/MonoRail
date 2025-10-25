#include "RenderPipeline.h"

#include "MMath.h"

RenderPipeline::RenderPipeline()
{
}

RenderPipeline::~RenderPipeline()
{
    if (m_camera_data_ubo)
        glDeleteBuffers(1, &m_camera_data_ubo);
    if (m_light_data_ubo)
        glDeleteBuffers(1, &m_light_data_ubo);
    if (m_fog_data_ubo)
        glDeleteBuffers(1, &m_fog_data_ubo);
}

void RenderPipeline::init(const entt::registry& _registry)
{
    Shader* _phong_shader     = ResourceManager::instance().get_shader("phong"          );
    Shader* _skeleton_shader  = ResourceManager::instance().get_shader("skeleton"       );
    Shader* _fog_plane_shader = ResourceManager::instance().get_shader("fog_plane"      );
    Shader* _depth_quad       = ResourceManager::instance().get_shader("depth_quad"     );
    Shader* _depth_of_field   = ResourceManager::instance().get_shader("depth_of_field" );
    Shader* _screen_quad      = ResourceManager::instance().get_shader("screen_quad"    );
    Shader* _ui_shader        = ResourceManager::instance().get_shader("ui"             );
    Shader* _ui_texture       = ResourceManager::instance().get_shader("ui_texture"     );
    Shader* _text_shader      = ResourceManager::instance().get_shader("text"           );
    Shader* _grass_shader     = ResourceManager::instance().get_shader("instance"       );
    Shader* _shell_shader     = ResourceManager::instance().get_shader("shell"          );
    Shader* _object_instance  = ResourceManager::instance().get_shader("object_instance");
    Shader* _aabb_shader      = ResourceManager::instance().get_shader("aabb");

    _phong_shader->use();
    _phong_shader->block_bind("CameraData"           , 0);
    _phong_shader->block_bind("DirectionalLightBlock", 1);
    _phong_shader->block_bind("FogDataBlock"         , 2);
    _phong_shader->set_float ("u_shininess", 100.0f);
    _phong_shader->set_vec3  ("u_color", glm::vec3(1.0f, 1.0f, 1.0f));

    _grass_shader->use();
    _grass_shader->block_bind("CameraData"           , 0);

    _shell_shader->use();
    _shell_shader->block_bind("CameraData"           , 0);

    _aabb_shader->use();
    _aabb_shader->block_bind("CameraData"           , 0);

    _skeleton_shader->use();
    _skeleton_shader->block_bind("CameraData"           , 0);
    _skeleton_shader->block_bind("DirectionalLightBlock", 1);
    _skeleton_shader->block_bind("FogDataBlock"         , 2);
    _skeleton_shader->set_float("u_shininess", 100.0f);

    _fog_plane_shader->use();
    _fog_plane_shader->block_bind("CameraData"           , 0);
    _fog_plane_shader->block_bind("DirectionalLightBlock", 1);
    _fog_plane_shader->block_bind("FogDataBlock"         , 2);
    _fog_plane_shader->set_float("u_shininess", 100.0f);
    _fog_plane_shader->set_vec2("u_screen_size", glm::vec2(1600, 1200));

    _depth_quad->use();
    _depth_quad->set_int("u_depth_texture", 0);
    _depth_quad->set_float("near_plane", 1);
    _depth_quad->set_float("far_plane", 550);

    _depth_of_field->use();
    _depth_of_field->set_int("u_depth_texture", 0);
    _depth_of_field->set_float("near_plane", 1);
    _depth_of_field->set_float("far_plane", 550);

    auto& _camera = _registry.ctx().get<Camera>();
    _depth_of_field->set_float("u_focus_dist" , _camera.focus_distance);
    _depth_of_field->set_float("u_focus_range", _camera.focus_range);


    _screen_quad->use();
    _screen_quad->set_int("u_screen_texture", 0);

    _object_instance->use();
    _object_instance->set_vec4("u_color", glm::vec4(0.0f, 0.0f, 1.0f, 0.1f));

    glm::mat4 _ortho_proj = glm::ortho(
        0.0f,
        static_cast<float>(g_app_config.SCREEN_WIDTH),
        static_cast<float>(g_app_config.SCREEN_HEIGHT),
        0.0f,
        -1.0f, 1.0f);


    _ui_shader->use();
    _ui_shader->set_mat4_uniform_projection(_ortho_proj);
    _ui_texture->use();
    _ui_texture->set_mat4_uniform_projection(_ortho_proj);
    _text_shader->use();
    _text_shader->set_mat4_uniform_projection(_ortho_proj);

    m_framebuffer                  = FrameBuffer(g_app_config.SCREEN_WIDTH, g_app_config.SCREEN_HEIGHT, true);
    m_depth_framebuffer            = FrameBuffer(g_app_config.SCREEN_WIDTH, g_app_config.SCREEN_HEIGHT, true);
    m_depth_shadow_map_framebuffer = FrameBuffer(2 * g_app_config.SCREEN_WIDTH, 2 * g_app_config.SCREEN_HEIGHT, true);
    //m_depth_shadow_map_framebuffer = FrameBuffer(2048, 2048, true);

    m_framebuffer.init();
    m_framebuffer.attach_color_texture();
    m_framebuffer.attach_depth_texture();

    m_depth_framebuffer.init();
    m_depth_framebuffer.attach_color_texture();
    m_depth_framebuffer.attach_depth_texture();

    m_depth_shadow_map_framebuffer.init();
    m_depth_shadow_map_framebuffer.attach_depth_texture();

    Quad _quad;
    m_screen_mesh = _quad.screen_vertices_to_mesh();
    m_screen_mesh_renderer.load_mesh      (&m_screen_mesh);
    m_screen_mesh_renderer.set_buffer_data(&m_screen_mesh);

    //Gen camera ubo
    glGenBuffers(1, &m_camera_data_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, m_camera_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraData), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //Gen light ubo
    glGenBuffers(1, &m_light_data_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, m_light_data_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //Gen fog ubo
    // glGenBuffers(1, &m_fog_data_ubo);
    // glBindBuffer(GL_UNIFORM_BUFFER, m_fog_data_ubo);
    // glBufferData(GL_UNIFORM_BUFFER, sizeof(FogData), nullptr, GL_STATIC_DRAW); // or nullptr if updating later
    // glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //Camera
    CameraData camera_data = _registry.ctx().get<Camera>().get_camera_data();

    glBindBuffer   (GL_UNIFORM_BUFFER, m_camera_data_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), &camera_data);
    glBindBuffer   (GL_UNIFORM_BUFFER, 0);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_camera_data_ubo);

    auto _directional_light_view = _registry.view<DirectionalLight>();
    for (auto _e : _directional_light_view)
    {
        auto& _directional_light = _registry.get<DirectionalLight>(_e);
        auto _light_data         = _directional_light.to_light_data();

        glBindBuffer   (GL_UNIFORM_BUFFER, m_light_data_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData), &_light_data);
        glBindBuffer   (GL_UNIFORM_BUFFER, 0);

        glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_light_data_ubo);
    }

    //Fog
    // m_fog_data.fogColor   = glm::vec3(0.5f, 0.5f, 0.5f); // gray fog
    // m_fog_data.fogStart   = -10.0f;
    // m_fog_data.fogEnd     = -4.0f;
    // m_fog_data.fogDensity = 3.0f;
    // m_fog_data.pad        = 0.0f; // padding must be set
    //
    // glBindBuffer   (GL_UNIFORM_BUFFER, m_fog_data_ubo);
    // glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(FogData), &m_fog_data);
    // glBindBuffer   (GL_UNIFORM_BUFFER, 0);
    //
    // glBindBufferBase(GL_UNIFORM_BUFFER, 2, m_fog_data_ubo);

    SkeletonMesh* _skeleton_mesh = ResourceManager::instance().get_first_skeleton_mesh("test_idle_skeleton");
    m_skeleton_mesh_renderer.load_mesh      (_skeleton_mesh);
    m_skeleton_mesh_renderer.set_buffer_data(_skeleton_mesh);

    m_animation = Animation("../res/models/test_idle_skeleton.fbx", _skeleton_mesh);
    m_animator  = Animator(&m_animation);

    m_ui_renderer.init();

    AABB _aabb;
    _aabb.min = glm::vec3(-0.5f, -0.5f, -0.5f);
    _aabb.max = glm::vec3( 0.5f,  0.5f,  0.5f);
    m_gizmos_renderer.create_aabb_gizmos(_aabb);
}

void RenderPipeline::render(const entt::registry& _registry)
{
    //m_animator.update_animation(0.016f);//!!!temp
    // Shader* _grass_shader     = ResourceManager::instance().get_shader("instance"   );
    // _grass_shader->use();
    // _grass_shader->set_float("u_time", SDL_GetTicks() / 500.0f);
    //
    // Shader* _shell_shader     = ResourceManager::instance().get_shader("shell"   );
    // _shell_shader->use();
    // _shell_shader->set_float("u_time", SDL_GetTicks() / 16000.0f);

    auto _mesh_view = _registry.view<Transform, MeshRenderer, Material>();

#pragma region reneder shadow make_public

    auto _light_view = _registry.view<DirectionalLight>();
    for (auto _e : _light_view)
    {
        auto& _light = _registry.get<DirectionalLight>(_e);

        if (!_light.cast_shadow)
            continue;

        CameraData _camera_data;
        _camera_data.viewPos    = _light.position;
        _camera_data.projection = _light.get_projection_matrix();
        _camera_data.view       = _light.get_view_matrix();

        glBindBuffer   (GL_UNIFORM_BUFFER, m_camera_data_ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), &_camera_data);
        glBindBuffer   (GL_UNIFORM_BUFFER, 0);

        m_depth_shadow_map_framebuffer.bind();

        glViewport  (0, 0, 2 * g_app_config.SCREEN_WIDTH, 2 * g_app_config.SCREEN_HEIGHT);
        glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
        glClear     (GL_DEPTH_BUFFER_BIT);

        for (auto _e : _mesh_view)
        {
            auto& _transform     = _registry.get<Transform>   (_e);
            auto& _mesh_renderer = _registry.get<MeshRenderer>(_e);
            auto& _material      = _registry.get<Material>    (_e);

            if (!_material.cast_shadow)
                continue;

            Shader* _found_shader = ResourceManager::instance().get_shader(_material.shader_id);
            _found_shader->use();
            _found_shader->set_mat4_uniform_model(_transform.world_mat);

            if (_material.depth_write)
                _mesh_renderer.draw();
        }

        m_depth_shadow_map_framebuffer.unbind();
    }

#pragma endregion

    // glm::mat4 _camera_view  = _registry.ctx().get<Camera>().get_view_matrix();
    //
    // glBindBuffer   (GL_UNIFORM_BUFFER, m_camera_data_ubo);
    // glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(_camera_view));
    // glBindBuffer   (GL_UNIFORM_BUFFER, 0);

    CameraData _camera_data = _registry.ctx().get<Camera>().get_camera_data();

    glBindBuffer   (GL_UNIFORM_BUFFER, m_camera_data_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraData), &_camera_data);
    glBindBuffer   (GL_UNIFORM_BUFFER, 0);

#pragma region render color n depth texture pass
    m_depth_framebuffer.bind();

    glViewport  (0, 0, g_app_config.SCREEN_WIDTH, g_app_config.SCREEN_HEIGHT);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear     (GL_DEPTH_BUFFER_BIT);

    for (auto _e : _mesh_view)
    {
        auto& _transform     = _registry.get<Transform>   (_e);
        auto& _mesh_renderer = _registry.get<MeshRenderer>(_e);
        auto& _material      = _registry.get<Material>    (_e);

        Shader* _found_shader = ResourceManager::instance().get_shader(_material.shader_id);
        _found_shader->use();
        _found_shader->set_mat4_uniform_model(_transform.world_mat);

        if (_material.depth_write)
            _mesh_renderer.draw();
    }

    m_depth_framebuffer.unbind();
#pragma endregion

#pragma region render to framebuffer pass
    m_framebuffer.bind();

    glViewport  (0, 0, g_app_config.SCREEN_WIDTH, g_app_config.SCREEN_HEIGHT);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable    (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (auto _e : _mesh_view)
    {
        auto& _transform     = _registry.get<Transform>   (_e);
        auto& _mesh_renderer = _registry.get<MeshRenderer>(_e);
        auto& _material      = _registry.get<Material>    (_e);

        Shader* _found_shader = ResourceManager::instance().get_shader(_material.shader_id);
        _found_shader->use();
        _found_shader->set_vec3("u_color", _material.diffuse_color);
        _found_shader->set_mat4_uniform_model(_transform.world_mat);

        if (_material.shader_id == "phong")
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_depth_shadow_map_framebuffer.get_depth_texture());
        }

        if (!_material.depth_write)
        {
            glDepthMask(GL_FALSE);

            auto& _camera = _registry.ctx().get<Camera>();

            _found_shader->use();
            _found_shader->set_int  ("u_color_texture", 0);
            _found_shader->set_int  ("u_depth_texture", 1);
            _found_shader->set_float("u_near_plane", _camera.near_plane);
            _found_shader->set_float("u_far_plane" , _camera.far_plane);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_depth_framebuffer.get_color_texture());
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_depth_framebuffer.get_depth_texture());
        }

        if (_material.diffuseMap != 0)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _material.diffuseMap);
            _found_shader->set_int("grassPattern", 0);

            _mesh_renderer.draw();

            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else
        {
            _mesh_renderer.draw();
        }

         if (!_material.depth_write)
             glDepthMask(GL_TRUE);    // restore
    }

    auto _gizmos_view = _registry.view<Transform, AABB, GizmosRenderer>();
    for (auto _e : _gizmos_view)
    {
        auto& _transform      = _registry.get<Transform>(_e);
        auto& _aabb           = _registry.get<AABB>(_e);
        auto& _gizmo_renderer = _registry.get<GizmosRenderer>(_e);

        Shader* _aabb_shader = ResourceManager::instance().get_shader("aabb");
        _aabb_shader->use();
        _aabb_shader->set_mat4_uniform_model(_transform.world_mat);

        _gizmo_renderer.draw();
    }

    glDisable (GL_BLEND);

    //Render skeleton mesh renderer
    // glm::mat4 _model = glm::mat4(1.0f);
    // _model = glm::translate(_model, glm::vec3(5.0f, 0.0f, 5.0f));
    // _model = glm::scale    (_model, glm::vec3(0.05f));
    //
    // Shader* _skeleton_shader = ResourceManager::instance().get_shader("skeleton");
    // _skeleton_shader->use();
    //
    // auto transforms = m_animator.get_final_bone_matrices();
    // for (int i = 0; i < transforms.size(); ++i)
    //     _skeleton_shader->set_mat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
    //
    // _skeleton_shader->set_mat4_uniform_model(_model);
    // m_skeleton_mesh_renderer.draw();

    m_framebuffer.unbind();
#pragma endregion

#pragma region render screen quad pass

    glViewport  (0, 0, 2 * g_app_config.SCREEN_WIDTH, 2 * g_app_config.SCREEN_HEIGHT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!display_depth)
    {
        Shader* _screen_quad = ResourceManager::instance().get_shader("screen_quad");
        _screen_quad->use();
        _screen_quad->set_int("u_screen_texture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_framebuffer.get_color_texture());
    }
    else
    {
        if (!display_dof)
        {
            Shader* _depth_quad = ResourceManager::instance().get_shader("depth_quad");
            _depth_quad->use();
            _depth_quad->set_int("u_depth_texture", 0);

            if (!display_shadow_map)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_depth_framebuffer.get_depth_texture());
            }
            else
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_depth_shadow_map_framebuffer.get_depth_texture());
            }
        }
        else
        {
            Shader* _depth_quad = ResourceManager::instance().get_shader("depth_of_field");
            _depth_quad->use();
            _depth_quad->set_int("u_depth_texture", 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_depth_framebuffer.get_depth_texture());
        }
    }

    m_screen_mesh_renderer.draw();

#pragma endregion

#pragma region render UI

    glClear(GL_DEPTH_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    MGUI::draw_texture({0.0f, 0.0f}, { 200.0f, 200.0f }, m_depth_framebuffer.get_depth_texture());

    // auto& _camera = _registry.ctx().get<Camera>();
    // glm::mat4 _view_mat = _camera.get_view_matrix      ();
    // glm::mat4 _proj_mat = _camera.get_projection_matrix();
    //
    // for (auto _e : _mesh_view)
    // {
    //     auto& _transform = _registry.get<Transform>(_e);
    //
    //     glm::vec3 _screen_pos = Util::world_to_screen(_transform.position, _view_mat, _proj_mat, g_app_config.SCREEN_WIDTH, g_app_config.SCREEN_HEIGHT);
    //     _screen_pos.x -= 60.0f;
    //     _screen_pos.y += 60.0f;
    //
    //     MGUI::draw_text("C_1000_0000", { _screen_pos.x, _screen_pos.y } , { 1, 1,1, 1});
    // }

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

#pragma endregion
}

void RenderPipeline::render_default(const entt::registry& _registry)
{
    m_framebuffer.bind();

    glViewport  (0, 0, g_app_config.SCREEN_WIDTH, g_app_config.SCREEN_HEIGHT);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable    (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto _mesh_view = _registry.view<Transform, MeshRenderer, Material>();

    for (auto _e : _mesh_view)
    {
        auto& _transform     = _registry.get<Transform>   (_e);
        auto& _mesh_renderer = _registry.get<MeshRenderer>(_e);
        auto& _material      = _registry.get<Material>    (_e);

        Shader* _found_shader = ResourceManager::instance().get_shader(_material.shader_id);
        _found_shader->use();
        _found_shader->set_mat4_uniform_model(_transform.world_mat);

        _mesh_renderer.draw();
    }

    m_framebuffer.unbind();

    glDisable (GL_BLEND);

    glViewport  (0, 0, 2* g_app_config.SCREEN_WIDTH, 2 * g_app_config.SCREEN_HEIGHT);
    glClearColor(0.1f, 0.0f, 0.0f, 1.0f);
    glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader* _screen_quad = ResourceManager::instance().get_shader("screen_quad");

    _screen_quad->use();
    _screen_quad->set_int("u_screen_texture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_framebuffer.get_color_texture());

    m_screen_mesh_renderer.draw_mesh();
}

void RenderPipeline::update_light_ubo(DirectionalLight &_directional_light)
{
    _directional_light.direction = glm::normalize(_directional_light.direction);

    auto _light_data = _directional_light.to_light_data();

    glBindBuffer   (GL_UNIFORM_BUFFER, m_light_data_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData), &_light_data);
    glBindBuffer   (GL_UNIFORM_BUFFER, 0);
}
