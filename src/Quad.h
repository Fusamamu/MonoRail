#ifndef QUAD_H
#define QUAD_H

#include "Mesh.h"
#include <cstring>

struct Quad
{
    // 4 vertices: position (3) | normal (3) | texcoord (2)
    float vertices[32] =
    {
        // positions        // normals       // texcoords
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // bottom-left
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // bottom-right
         0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, // top-right
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f  // top-left
    };

    unsigned int indices[6] =
    {
        0, 1, 2,  // first triangle
        2, 3, 0   // second triangle
    };

    Quad() = default;
    ~Quad() = default;

    Mesh to_mesh()
    {
        Mesh mesh;

        mesh.layout = {};
        mesh.layout.addElement(VertexAttribute::POSITION  , 0                      , sizeof(float) * 3, GL_FLOAT, 3);
        mesh.layout.addElement(VertexAttribute::NORMAL    , sizeof(float) * 3      , sizeof(float) * 3, GL_FLOAT, 3);
        mesh.layout.addElement(VertexAttribute::TEXCOORD_0, sizeof(float) * 6      , sizeof(float) * 2, GL_FLOAT, 2);
        mesh.layout.stride = sizeof(float) * 8;

        mesh.vertex_buffer.resize(sizeof(vertices));
        std::memcpy(mesh.vertex_buffer.data(), vertices, sizeof(vertices));

        mesh.index_buffer.assign(indices, indices + 6);

        return mesh;
    }
};

#endif
