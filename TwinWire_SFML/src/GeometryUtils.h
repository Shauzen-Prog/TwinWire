#pragma once
#include <SFML/Graphics.hpp>

// Accessors compatible with SFML 3 FloatRect (position/size)
#define RECT_LEFT(r)   ((r).position.x)
#define RECT_TOP(r)    ((r).position.y)
#define RECT_RIGHT(r)  ((r).position.x + (r).size.x)
#define RECT_BOTTOM(r) ((r).position.y + (r).size.y)

// Liang–Barsky: segment vs AABB (declared here, implemented in GeometryUtils.cpp)
bool segmentIntersectsAABB(const sf::Vector2f& p0,
                           const sf::Vector2f& p1,
                           const sf::FloatRect& r);
