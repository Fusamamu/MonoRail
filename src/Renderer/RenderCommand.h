#ifndef RENDERCOMMAND_H
#define RENDERCOMMAND_H

#include "Material.h"

class MeshRenderer;

struct RenderCommand
{
    const Material    * material;
    const MeshRenderer* mesh_renderer;

    glm::mat4 model_mat;

    GLuint shader_map;
    GLuint texture;
    GLuint ao_map;
};

#endif
