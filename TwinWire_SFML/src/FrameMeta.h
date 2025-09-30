#pragma once
#include <SFML/Graphics/Rect.hpp>

struct FrameMeta
{
    sf::IntRect rect;   // {{left, top}, {width, height}}
    float pivotX = 0.f; // píxeles desde left, el Y del pivot es el borde inferior del rect
    float duration = 0.10f;
};
