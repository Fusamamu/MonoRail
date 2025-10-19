#ifndef TIME_H
#define TIME_H

namespace Time
{
    extern Uint64 now;
    extern Uint64 last;
    extern double delta;

    extern double fps;

    void init();
    void update();
}


#endif //TIME_H
