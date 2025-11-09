#ifndef UIRENDERER_H
#define UIRENDERER_H

#include "../PCH.h"
#include "../Renderer/Shader.h"

namespace MGUI
{
    class UIRenderer
    {
    public:
        UIRenderer();
        ~UIRenderer();

        void init();
        void draw();

    private:
        unsigned int m_vao, m_vbo, m_ebo;

        float ui_vertices[32] =
        {
            // position        // normal       // texcoords
            0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // top-left
            1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // top-right
            1.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // bottom-right
            0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,  0.0f, 1.0f  // bottom-left
        };
    };
}

#endif
