#ifndef GIZMOSRENDERER_H
#define GIZMOSRENDERER_H

#include "Core/Mesh.h"

struct AABB;

enum class GizmosType : int8_t
{
    NONE = -1,
    LINE = 0
};

class GizmosRenderer
{
public:
    GizmosType Type = GizmosType::NONE;

    GizmosRenderer();
    ~GizmosRenderer();

    void init(GizmosType _type);
    void create_aabb_gizmos(AABB& _aabb);

    void update_line_vertice(const std::vector<glm::vec3>& _line_vertices);

    void draw() const;
private:
    GLuint m_vbo, m_vao;

    std::vector<LineVertex> m_line_vertices;
};

#endif
