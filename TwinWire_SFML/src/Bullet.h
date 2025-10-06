#pragma once
#include <SFML/Graphics.hpp>
#include <memory> 

struct Bullet
{
    bool active{false};
    bool textured{false};
    sf::Vector2f pos{};
    sf::Vector2f vel{}; // px/s
    float radius{4.f};
    float life{0.f};
    float maxLife{2.f}; // en segundos
    
    std::unique_ptr<sf::Sprite> sprite;
};
