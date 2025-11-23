#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include <GL/glew.h>

#include "Renderer/MeshRenderer.h"
#include "Renderer/FrameBuffer.h"
#include "Renderer/GBuffer.h"
#include "Renderer/SkeletonMeshRenderer.h"
#include "Renderer/GizmosRenderer.h"

#include "Asset/Texture.h"
#include "RenderQueue.h"
#include "Animation/Animation.h"

#include "UI/UI.h"

struct DirectionalLight;

enum class DisplayRenderMode
{
    NONE           = 0,
    DEPTH          = 1,
    WORLD_POSITION = 2,
    WORLD_NORMAL   = 3,
};

class RenderPipeline
{
public:
    DisplayRenderMode display_render_mode = DisplayRenderMode::NONE;

    float slice = 0.0f;
    int voxel_resolution = 128;
    Texture3D voxel_texture;

    bool display_depth      = false;
    bool display_dof        = false;
    bool display_shadow_map = false;

    RenderPipeline();
    ~RenderPipeline();

    void init          (const entt::registry& _registry);
    void render        (const entt::registry& _registry);

    void generate_fog ();
    void render_ao_map();

    void update_light_ubo  (DirectionalLight& _directional_light);
    void update_line_gizmos(const std::vector<glm::vec3>& _line_gizmos){
        m_gizmos_renderer.update_line_vertice(_line_gizmos);
    }

private:
    GLuint m_voxel_ao_texture_3d, fbo;

    GLuint m_camera_data_ubo;
    GLuint m_light_data_ubo ;
    GLuint m_fog_data_ubo   ;

    RenderQueue m_render_queue;

    FrameBuffer m_framebuffer                 ;
    FrameBuffer m_depth_framebuffer           ;
    FrameBuffer m_depth_shadow_map_framebuffer;
    FrameBuffer m_voxel_ao_framebuffer        ;

    MUG::Renderer::GBuffer m_g_buffer;

    MUG::Mesh            m_screen_mesh;
    MeshRenderer         m_screen_mesh_renderer;

    Texture   m_perlin_noise_texture;
    Texture3D m_voxel_ambient_texture_3d;

    MGUI::UIRenderer m_ui_renderer;
    GizmosRenderer   m_gizmos_renderer;

    Animation            m_animation;
    Animator             m_animator;
    SkeletonMeshRenderer m_skeleton_mesh_renderer;
};

#endif
