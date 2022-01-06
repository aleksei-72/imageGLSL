#include <src/timer/timer.h>

Timer::Timer()
{
    time = clock();
}

clock_t Timer::getElapsedTime()
{
    clock_t t = clock();
    clock_t delta = t - time;
    time = t;
    return delta /(float) CLOCKS_PER_SEC * 1000;
}
