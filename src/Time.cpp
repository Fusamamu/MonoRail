#include "Time.h"

namespace Time
{
    Uint64 now;
    Uint64 last;
    double delta;
    float delta_f;
    double fps;

    void init()
    {
       now   = SDL_GetPerformanceCounter();
       last  = 0;
       delta = 0.0;
    }

    void update()
    {
        last  = now;
        now   = SDL_GetPerformanceCounter();
        delta_f = ((now - last) * 1000 / (double)SDL_GetPerformanceFrequency());
        delta  = (double)delta_f;
        double _deltaSeconds = (now - last) / (double)SDL_GetPerformanceFrequency();
        fps = 1.0 / _deltaSeconds;
    }
}