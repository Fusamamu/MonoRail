#include "RenderPipeline.h"

RenderPipeline::RenderPipeline()
{
}

RenderPipeline::~RenderPipeline()
{

}

void RenderPipeline::init()
{
    m_framebuffer       = FrameBuffer(g_app_config.SCREEN_WIDTH, g_app_config.SCREEN_HEIGHT, true);
    m_depth_framebuffer = FrameBuffer(g_app_config.SCREEN_WIDTH, g_app_config.SCREEN_HEIGHT, true);

    m_framebuffer.init();
    m_framebuffer.attach_color_texture();
    m_framebuffer.attach_depth_texture();

    m_depth_framebuffer.init();
    m_depth_framebuffer.attach_color_texture();
    m_depth_framebuffer.attach_depth_texture();

    Quad _quad;
    Mesh _mesh = _quad.screen_vertices_to_mesh();
    m_screen_mesh_renderer.load_mesh      (&_mesh);
    m_screen_mesh_renderer.set_buffer_data(&_mesh);
}

void RenderPipeline::render(const entt::registry& _registry)
{
#pragma region render color n depth texture pass
    m_depth_framebuffer.bind();

    glViewport  (0, 0, 1600, 1200);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear     (GL_DEPTH_BUFFER_BIT);

    auto _mesh_view_0 = _registry.view<Transform, MeshRenderer, Material>();
    for (auto _e : _mesh_view_0)
    {
        auto& _transform     = _registry.get<Transform>   (_e);
        auto& _mesh_renderer = _registry.get<MeshRenderer>(_e);
        auto& _material      = _registry.get<Material>    (_e);

        Shader* _default_shader = ResourceManager::instance().get_shader(_material.shader_id);
        _default_shader->use();
        _default_shader->set_mat4_uniform_model(_transform.world_mat);

        if (_material.depth_write)
            _mesh_renderer.draw();
    }

    m_depth_framebuffer.unbind();
#pragma endregion

#pragma region render to framebuffer pass
    m_framebuffer.bind();

    glViewport  (0, 0, 1600, 1200);
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto _mesh_view = _registry.view<Transform, MeshRenderer, Material>();
    for (auto _e : _mesh_view)
    {
        auto& _transform     = _registry.get<Transform>   (_e);
        auto& _mesh_renderer = _registry.get<MeshRenderer>(_e);
        auto& _material      = _registry.get<Material>    (_e);

        Shader* _default_shader = ResourceManager::instance().get_shader(_material.shader_id);
        _default_shader->use();
        _default_shader->set_mat4_uniform_model(_transform.world_mat);

        if (!_material.depth_write)
        {
            glDepthMask(GL_FALSE);

            auto& _camera = _registry.ctx().get<Camera>();

            _default_shader->use();
            _default_shader->set_int("u_color_texture", 0);
            _default_shader->set_int("u_depth_texture", 1);
            _default_shader->set_float("u_near_plane", _camera.near_plane);
            _default_shader->set_float("u_far_plane" , _camera.far_plane);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_depth_framebuffer.get_color_texture());

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_depth_framebuffer.get_depth_texture());
        }

        _mesh_renderer.draw();

        if (!_material.depth_write)
            glDepthMask(GL_TRUE);    // restore
    }

    m_framebuffer.unbind();
#pragma endregion

#pragma region render screen quad pass
    glViewport  (0, 0, 1600 * 2, 1200 * 2);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear     (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Quad _quad;
    Mesh _mesh = _quad.screen_vertices_to_mesh();

    MeshRenderer _mesh_renderer;
    _mesh_renderer.load_mesh      (&_mesh);
    _mesh_renderer.set_buffer_data(&_mesh);

    if (!m_display_depth)
    {
        Shader* _screen_quad = ResourceManager::instance().get_shader("screen_quad");
        _screen_quad->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_framebuffer.get_color_texture());
    }
    else
    {
        Shader* _depth_quad = ResourceManager::instance().get_shader("depth_quad");
        _depth_quad->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_depth_framebuffer.get_depth_texture());
    }
    _mesh_renderer.draw();
#pragma endregion
}