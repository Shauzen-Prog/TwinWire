#pragma once
#include <SFML/Graphics.hpp>

// Base interface for UI widgets (SOLID-friendly, KISS)
class IWidget
{
public:
    virtual ~IWidget() = default;

    // Dispatch SFML events to the widget (window is provided for coord mapping)
    virtual void handleEvent(const sf::Event& ev, const sf::RenderWindow& window) = 0;

    // Per-frame logic (dt in seconds)
    virtual void update(float dt) = 0;

    // Render the widget
    virtual void draw(sf::RenderTarget& target) const = 0;
};
