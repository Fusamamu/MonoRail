#ifndef GIZMOSRENDERER_H
#define GIZMOSRENDERER_H

#include "Component.h"

class GizmosRenderer
{
public:
    GizmosRenderer();
    ~GizmosRenderer();

    void create_aabb_gizmos(AABB& _aabb);

    void draw() const;
private:
    GLuint m_vbo, m_vao;
};

#endif
