#pragma once
#include <SFML/Graphics.hpp>
#include "IChockeable.h"

struct IChokeQuery
{
    virtual ~IChokeQuery() = default;
    // devuelve el chockeable impactado o nullptr. outHit = punto de impacto
    virtual IChockeable* RaycastChoke(const sf::Vector2f& a,
                                      const sf::Vector2f& b,
                                      sf::Vector2f& outHit) = 0;
};
