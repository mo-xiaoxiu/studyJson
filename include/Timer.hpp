#ifndef TIMER_H_
#define TIMER_H_

#include <iostream>
#include <chrono>

class Timer
{
public:
    Timer()
    {
        m_startTimerPoint = std::chrono::high_resolution_clock::now();
    }

    void stop()
    {
        auto endTimerPoint = std::chrono::high_resolution_clock::now();
        auto start = std::chrono::time_point_cast<std::chrono::microseconds>(
            m_startTimerPoint).time_since_epoch().count();
        auto end = std::chrono::time_point_cast<std::chrono::microseconds>(
        endTimerPoint).time_since_epoch().count();

        auto duration = end - start; //us
        auto ms = duration * 0.001; //ms
        printf("%ld us (%lf ms)\n", duration, ms);
        fflush(stdout);
    }

    ~Timer()
    {
        stop();
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimerPoint;        
};

#endif