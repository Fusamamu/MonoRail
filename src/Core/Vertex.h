#ifndef VERTEX_H
#define VERTEX_H

#define MAX_BONE_INFLUENCE 4

struct LineVertex
{
    glm::vec3 position;
    glm::vec3 color;
};

struct Vertex_PNT
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Vertex_Bone
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    int   BoneIDs[MAX_BONE_INFLUENCE];
    float Weights[MAX_BONE_INFLUENCE];

    Vertex_Bone()
    {
        for(int _i = 0; _i < 4; _i++)
        {
            BoneIDs[_i] = -1;
            Weights[_i] = 0.0f;
        }
    }

    void add_bone_data(int _bone_id, float _weight)
    {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
        {
            if (Weights[i] == 0.0f)
            {
                BoneIDs[i] = _bone_id;
                Weights[i] = _weight;
                return;
            }
        }
    }
};

enum class VertexAttribute
{
    POSITION,
    NORMAL,
    TEXCOORD_0,
    COLOR,
    TANGENT
};

struct VertexElement
{
    VertexAttribute attribute;
    uint32_t offset;    // offset in bytes in the vertex struct
    uint32_t size;      // size in bytes of attribute (e.g. 12 for vec3 float)
    GLenum glType;      // GL_FLOAT, GL_UNSIGNED_BYTE etc.
    int components;     // number of components (e.g. 3 for vec3)
    bool normalized;    // normalized flag for GL
};

struct VertexLayout
{
    std::vector<VertexElement> elements;

    uint32_t stride;

    void add_element(VertexAttribute _attr, uint32_t _offset, uint32_t _size, GLenum _glType, int _components, bool _normalized = false)
    {
        elements.push_back({ _attr, _offset, _size, _glType, _components, _normalized });
    }
};


#endif
