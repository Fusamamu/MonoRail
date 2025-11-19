#include "Transform.h"

namespace Component
{
    Transform::Transform():
          position({ 0.0f, 0.0f, 0.0f }),
          rotation({ 0.0f, 0.0f, 0.0f }),
          scale   ({ 1.0f, 1.0f, 1.0f })
    {

    }

    Transform::~Transform()
    {

    }

    glm::mat4 Transform::get_local_mat4() const
    {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);

        // rotation order: Y (yaw), X (pitch), Z (roll) — you can change if needed
        glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1,0,0));
        glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0,1,0));
        glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0,0,1));
        glm::mat4 rotationMat = rotZ * rotY * rotX;

        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);

        return translation * rotationMat * scaling;
    }
}
