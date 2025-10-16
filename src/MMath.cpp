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

