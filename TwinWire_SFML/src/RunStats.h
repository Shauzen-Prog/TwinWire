#pragma once
#include <string>

namespace RunStats
{
    void reset();
    void start();
    void stop();
    void update(float dt);
    void addDeath();

    int deaths();
    float elapsed();

    bool isRunning();
}


