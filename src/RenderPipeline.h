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

class RenderPipeline
{
public:
    RenderPipeline();
    ~RenderPipeline();

    void init();

    void render(const entt::registry& _registry);
private:
    FrameBuffer m_framebuffer;
    FrameBuffer m_depth_framebuffer;

    MeshRenderer m_screen_mesh_renderer;


    bool m_display_depth = false;
};

#endif
