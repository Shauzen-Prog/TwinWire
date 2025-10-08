#pragma once
#include <cstdlib>

// [a,b] entero (incluye extremos)
inline int irand(int a, int b)
{
    return a + std::rand() % (b - a + 1);
}

// [a,b] float (b no incluido)
inline float frand(float a, float b)
{
    const float t = static_cast<float>(std::rand()) / (static_cast<float>(RAND_MAX) + 1.0f);
    return a + t * (b - a);   
}
