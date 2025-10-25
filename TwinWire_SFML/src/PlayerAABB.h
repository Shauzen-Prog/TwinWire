#pragma once
#include <SFML/Graphics.hpp>

// Elige UNA sola pareja de constantes que te haya quedado bien en el tuner.
inline constexpr sf::Vector2f kPlayerHbSize   {16.f, 22.f};
inline constexpr sf::Vector2f kPlayerHbOffset { 0.f, -6.f };

inline sf::FloatRect makePlayerAABB(sf::Vector2f feet) {
    const sf::Vector2f origin = feet + kPlayerHbOffset - kPlayerHbSize * 0.5f;
    return {origin, kPlayerHbSize};
}
