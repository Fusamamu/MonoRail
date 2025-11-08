#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include "PCH.h"
#include "Camera.h"
#include "Quad.h"
#include "MeshRenderer.h"
#include "FrameBuffer.h"
#include "ApplicationConfig.h"
#include "Shader.h"
#include "Component.h"
#include "Asset/AssetManager.h"
#include "SkeletonMeshRenderer.h"
#include "Animation.h"
#include "UI/UI.h"
#include "GizmosRenderer.h"
#include "MMath.h"
#include "Profiler.h"
#include "Texture.h"

class RenderPipeline
{
public:
    bool display_depth      = false;
    bool display_dof        = false;
    bool display_shadow_map = false;

    RenderPipeline();
    ~RenderPipeline();

    void init          (const entt::registry& _registry);
    void render        (const entt::registry& _registry);
    void render_default(const entt::registry& _registry);

    void update_light_ubo(DirectionalLight& _directional_light);

    void update_line_gizmos(const std::vector<glm::vec3>& _line_gizmos)
    {
        m_gizmos_renderer.update_line_vertice(_line_gizmos);
    }
private:
    FrameBuffer m_framebuffer;
    FrameBuffer m_depth_framebuffer;
    FrameBuffer m_depth_shadow_map_framebuffer;

    GLuint m_camera_data_ubo;
    GLuint m_light_data_ubo ;
    GLuint m_fog_data_ubo   ;

    Mesh         m_screen_mesh;
    MeshRenderer m_screen_mesh_renderer;

    SkeletonMeshRenderer m_skeleton_mesh_renderer;

    Animation m_animation;
    Animator m_animator;

    Texture m_perlin_noise_texture;

    MGUI::UIRenderer m_ui_renderer;

    GizmosRenderer m_gizmos_renderer;
};

#endif
