#pragma once
#include <SFML/Graphics.hpp>
#include "IWidget.h"

class VolumeSlider : public IWidget
{
public:
    VolumeSlider();

    void setRect(const sf::FloatRect& r); // posición y tamaño
    void setValue(float v);               // [0..1]
    float value() const { return m_value; }

    // IWidget
    void handleEvent(const sf::Event& ev, const sf::RenderWindow& window) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;

private:
    sf::FloatRect m_rect{ {100.f,100.f}, {240.f, 16.f} };
    float m_value{1.f};
    bool  m_drag{false};

    // Visual simple
    sf::RectangleShape m_bar;
    sf::RectangleShape m_fill;
    sf::CircleShape    m_knob;

    void refreshVisual();
};
