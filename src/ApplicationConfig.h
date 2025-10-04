#ifndef APPLICATIONCONFIG_H
#define APPLICATIONCONFIG_H

#include "PCH.h"

struct ApplicationConfig
{
    unsigned int SCREEN_WIDTH  = 1600;
    unsigned int SCREEN_HEIGHT = 1200;

    glm::vec2 screen_size(bool _double = false)
    {
        if (_double)
            return glm::vec2(2 * SCREEN_WIDTH, 2 * SCREEN_HEIGHT);
        return glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT);
    }
};

extern ApplicationConfig g_app_config;

#endif
