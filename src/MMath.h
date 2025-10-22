#ifndef MMATH_H
#define MMATH_H

namespace EASE
{
    float linear(float t);
    float smoothstep(float t);
    float ease_in(float t);
    float ease_out(float t);
    float ease_in_out(float t);
    float ease_out_back(float t);

}

namespace Util
{
    glm::vec3 world_to_screen(
        const glm::vec3& worldPos,
        const glm::mat4& view,
        const glm::mat4& proj,
        int screenWidth,
        int screenHeight);
}
#endif
