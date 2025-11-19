#ifndef TRANSFORM_H
#define TRANSFORM_H

namespace Component
{
    struct Transform
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale   ;

        glm::mat4 world_mat;
        Transform();
        ~Transform();
        glm::mat4 get_local_mat4() const;
    };

}

#endif //TRANSFORM_H
