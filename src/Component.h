#ifndef COMPONENT_H
#define COMPONENT_H

#include "PCH.h"

struct Node
{
    std::string name;

    Node(const std::string& name) : name(name) {}
};

struct Transform
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transform():
        position({ 0.0f, 0.0f, 0.0f }),
        rotation({ 0.0f, 0.0f, 0.0f }),
        scale   ({ 0.0f, 0.0f, 0.0f })
    {

    }
};

struct Material
{
    glm::vec3 diffuseColor   {1.0f, 1.0f, 1.0f};  // base color
    glm::vec3 specularColor  {1.0f, 1.0f, 1.0f};  // highlight color
    float     shininess      {32.0f};             // specular exponent
    float     opacity        {1.0f};              // transparency

    unsigned int diffuseMap  = 0;  // OpenGL texture ID
    unsigned int specularMap = 0;  // OpenGL texture ID
    unsigned int normalMap   = 0;  // OpenGL texture ID

    Material() = default;

    Material(const glm::vec3& _diff, const glm::vec3& _spec, float _shin)
        : diffuseColor(_diff), specularColor(_spec), shininess(_shin)
    {

    }
};

#endif
