#ifndef TIMER_H
#define TIMER_H

#include <ctime>

class Timer
{
public:

    Timer();

    clock_t getElapsedTime();

private:

    clock_t time;
};

#endif // TIMER_H
