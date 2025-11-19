#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Material
{
    std::string shader_id;

    glm::vec4 rgba           { 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec3 diffuse_color  { 1.0f, 1.0f, 1.0f };  // base color
    glm::vec3 specularColor  { 1.0f, 1.0f, 1.0f };  // highlight color
    float     shininess      {32.0f};               // specular exponent
    float     opacity        {1.0f};                // transparency

    bool cast_shadow = true;
    bool depth_test  = true;
    bool depth_write = true;

    GLuint diffuseMap  = 0;
    GLuint specularMap = 0;
    GLuint normalMap   = 0;

    Material() = default;

    Material(const glm::vec3& _diff, const glm::vec3& _spec, float _shin)
        : diffuse_color(_diff), specularColor(_spec), shininess(_shin)
    {

    }
};

#endif
