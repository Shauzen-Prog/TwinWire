#pragma once
#include <SFML/Graphics.hpp>

class IBulletEmitter
{
public:
    virtual ~IBulletEmitter() = default;
    virtual void emit(sf::Vector2f pos, float angleRad, float speed) = 0; // lo hago puro asi fuerzo a implementar (como una interfaz en unity)
};
