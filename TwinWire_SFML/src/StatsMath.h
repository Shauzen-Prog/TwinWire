#pragma once
#include "HighScore.h"

struct RunAgg
{
    float timeSec{0.f};
    int   deaths{0};

    static RunAgg from(const ScoreEntry& s) { return RunAgg{ s.timeSec, s.deaths }; }
    
    RunAgg& operator+=(const RunAgg& other)
    {
        timeSec += other.timeSec;
        deaths  += other.deaths;
        return *this;
    }

    friend RunAgg operator+(RunAgg a, const RunAgg& b)
    {
        a += b;
        return a;
    }

    // para promedios
    friend RunAgg operator/(RunAgg a, std::size_t n)
    {
        if (n > 0) {
            a.timeSec /= static_cast<float>(n);
            a.deaths  = static_cast<int>(a.deaths / static_cast<int>(n)); // promedio enteros
        }
        return a;
    }
};
