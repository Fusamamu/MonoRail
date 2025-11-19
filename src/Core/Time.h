#ifndef M_TIME_H
#define M_TIME_H

#include <SDL2/SDL.h>

namespace Core
{
    namespace Time
    {
        extern Uint64 now;
        extern Uint64 last;
        extern float delta_f;
        extern double delta;

        extern double fps;

        void init();
        void update();
    }
}

#endif
