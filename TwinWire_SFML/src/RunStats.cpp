#include "RunStats.h"

namespace
{
    bool g_running = false;
    float g_time = 0.f;
    int g_deaths = 0;
}

void RunStats::reset() { g_running = false; g_time = 0.f; g_deaths = 0; }
void RunStats::start() { g_running = true; }
void RunStats::stop() { g_running = false; }
void RunStats::update(float dt) {if (g_running) g_time += dt;}
void RunStats::addDeath(){ g_deaths++;}
int RunStats::deaths(){ return g_deaths; }
float RunStats::elapsed(){ return g_time; }
bool RunStats::isRunning(){ return g_running; }

