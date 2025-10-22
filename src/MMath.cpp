#include "MMath.h"

#include <cmath>
#include <algorithm>

namespace EASE
{
    float linear(float t)
    {
        return std::clamp(t, 0.0f, 1.0f);
    }

    float smoothstep(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    float ease_in(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t;
    }

    float ease_out(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        return 1.0f - (1.0f - t) * (1.0f - t);
    }

    float ease_in_out(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        if (t < 0.5f)
            return 2.0f * t * t;
        else
            return 1.0f - std::pow(-2.0f * t + 2.0f, 2) / 2.0f;
    }

    float ease_out_back(float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        const float c1 = 1.70158f;
        const float c3 = c1 + 1.0f;
        return 1.0f + c3 * std::pow(t - 1.0f, 3) + c1 * std::pow(t - 1.0f, 2);
    }
}

namespace Util
{
    glm::vec3 world_to_screen(
       const glm::vec3& worldPos,
       const glm::mat4& view,
       const glm::mat4& proj,
       int screenWidth,
       int screenHeight)
    {
        // Transform world position to clip space
        glm::vec4 clipSpace = proj * view * glm::vec4(worldPos, 1.0f);

        // Perspective divide → NDC space (Normalized Device Coordinates)
        if (clipSpace.w == 0.0f)
            return glm::vec3(0.0f);

        glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w;

        // Convert from NDC (-1..1) to screen space (0..width, 0..height)
        glm::vec3 screen;
        screen.x = (ndc.x * 0.5f + 0.5f) * screenWidth;
        screen.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * screenHeight; // flip Y for OpenGL
        screen.z = ndc.z; // Depth value (−1 to +1)

        return screen;
    }
}
