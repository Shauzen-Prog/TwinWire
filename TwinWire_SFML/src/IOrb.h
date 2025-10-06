#pragma once
#include <SFML/System/Angle.inl>
#include <SFML/System/Vector2.hpp>

struct IOrb
{
    virtual ~IOrb() = default;
    virtual bool isActive() const = 0;
    virtual sf::Vector2f getPosition() const = 0;
};
