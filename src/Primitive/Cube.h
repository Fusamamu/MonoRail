#ifndef CUBE_H
#define CUBE_H

#include "../Core/Mesh.h"
#include <cstring>

struct Cube
{
    // 24 vertices (4 vertices per face × 6 faces)
    // Order: position (3) | normal (3) | texcoord (2)
    float vertices[24 * 8] = {
        // Front face (Z+)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
        // Back face (Z-)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  1.0f, 1.0f,
        // Left face (X-)
        -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
        // Right face (X+)
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f,
        // Top face (Y+)
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
        // Bottom face (Y-)
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f
    };

    unsigned int indices[36] = {
        0, 1, 2,  2, 3, 0,       // Front
        4, 5, 6,  6, 7, 4,       // Back
        8, 9,10, 10,11, 8,       // Left
       12,13,14, 14,15,12,       // Right
       16,17,18, 18,19,16,       // Top
       20,21,22, 22,23,20        // Bottom
    };

    Cube() = default;
    ~Cube() = default;

    Mesh to_mesh()
    {
        Mesh mesh;

        mesh.layout = {};
        mesh.layout.add_element(VertexAttribute::POSITION  , 0                      , sizeof(float) * 3, GL_FLOAT, 3);
        mesh.layout.add_element(VertexAttribute::NORMAL    , sizeof(float) * 3      , sizeof(float) * 3, GL_FLOAT, 3);
        mesh.layout.add_element(VertexAttribute::TEXCOORD_0, sizeof(float) * 6      , sizeof(float) * 2, GL_FLOAT, 2);
        mesh.layout.stride = sizeof(float) * 8;

        mesh.vertex_buffer.resize(sizeof(vertices));
        std::memcpy(mesh.vertex_buffer.data(), vertices, sizeof(vertices));

        mesh.index_buffer.assign(indices, indices + 36);

        return mesh;
    }
};

#endif
