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
#include "ResourceManager.h"
#include "SkeletonMeshRenderer.h"
#include "Animation.h"

class RenderPipeline
{
public:
    bool display_depth = false;

    RenderPipeline();
    ~RenderPipeline();

    void init  (const entt::registry& _registry);
    void render(const entt::registry& _registry);

    void update_light_ubo(DirectionalLight& _directional_light);
private:
    FrameBuffer m_framebuffer;
    FrameBuffer m_depth_framebuffer;

    GLuint m_camera_data_ubo;
    GLuint m_light_data_ubo ;
    GLuint m_fog_data_ubo   ;

    MeshRenderer m_screen_mesh_renderer;

    SkeletonMeshRenderer m_skeleton_mesh_renderer;

    Animation m_animation;
    Animator m_animator;

};

#endif
