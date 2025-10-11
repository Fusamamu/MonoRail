#ifndef SPHERE_H
#define SPHERE_H

#include "PCH.h"
#include "Mesh.h"

struct Sphere
{
    std::vector<float> vertices;      // position(3) | normal(3) | texcoord(2)
    std::vector<unsigned int> indices;

    Sphere(unsigned int sectorCount = 36, unsigned int stackCount = 18)
    {
        generate(sectorCount, stackCount);
    }

    ~Sphere() = default;

    Mesh to_mesh() const
    {
        Mesh mesh;

        mesh.layout = {};
        mesh.layout.add_element(VertexAttribute::POSITION  , 0                , sizeof(float) * 3, GL_FLOAT, 3);
        mesh.layout.add_element(VertexAttribute::NORMAL    , sizeof(float) * 3, sizeof(float) * 3, GL_FLOAT, 3);
        mesh.layout.add_element(VertexAttribute::TEXCOORD_0, sizeof(float) * 6, sizeof(float) * 2, GL_FLOAT, 2);
        mesh.layout.stride = sizeof(float) * 8;

        mesh.vertex_buffer.resize(vertices.size() * sizeof(float));
        std::memcpy(mesh.vertex_buffer.data(), vertices.data(), vertices.size() * sizeof(float));

        mesh.index_buffer = indices;

        return mesh;
    }

private:
    void generate(unsigned int sectorCount, unsigned int stackCount)
    {
        const float PI = 3.14159265359f;

        vertices.clear();
        indices.clear();

        for (unsigned int i = 0; i <= stackCount; ++i)
        {
            float stackAngle = PI / 2 - i * (PI / stackCount); // from pi/2 to -pi/2
            float xy = cosf(stackAngle);
            float z  = sinf(stackAngle);

            for (unsigned int j = 0; j <= sectorCount; ++j)
            {
                float sectorAngle = j * (2 * PI / sectorCount);

                float x = xy * cosf(sectorAngle);
                float y = xy * sinf(sectorAngle);

                // Position
                vertices.push_back(x * 0.5f);
                vertices.push_back(y * 0.5f);
                vertices.push_back(z * 0.5f);

                // Normal (same as position, since unit sphere)
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // TexCoords
                float u = (float)j / sectorCount;
                float v = (float)i / stackCount;
                vertices.push_back(u);
                vertices.push_back(v);
            }
        }

        // Indices
        for (unsigned int i = 0; i < stackCount; ++i)
        {
            unsigned int k1 = i * (sectorCount + 1);
            unsigned int k2 = k1 + sectorCount + 1;

            for (unsigned int j = 0; j < sectorCount; ++j, ++k1, ++k2)
            {
                if (i != 0)
                {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }

                if (i != (stackCount - 1))
                {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
    }
};

#endif
